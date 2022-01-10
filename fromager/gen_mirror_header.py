'''
Tool for generating C headers that match definitions from the host libraries.

The input is a file that looks like this:

```
include foo/bar.h

define FOO
define BAR

struct foo
    uint8_t x;
    uint32_t y;

struct foo
    uint32_t x;
    @ uint16_t _pad0;
    uint8_t y;

output_include stdint.h
output_guard FOO_BAR_INCLUDED
```

Header generation happens in two steps.  First, this script produces C source
code for a "generator program"; second, the generator program emits a C header
with the requested definitions.

The allowed directives are:

* `include header.h`: Add `#include <header.h>` to the generator program.
  Usually, `header.h` will be the header containing the definitions to be
  matched.

* `define NAME`: Match a `#define`'d constant.  The generator program will emit
  a `#define NAME VAL`, where `VAL` is the value used in the host library.

* `struct NAME`: Match a struct definition.  The `struct` directive is followed
  by an indented block giving the body of the struct.  The generator program
  will detect whether the layout of the provided struct definition matches the
  definition in the host library header.  If so, it will emit the definition;
  otherwise, it will report an error.  Two layouts match if every field has the
  same size and offset in both structs.  If there are multiple definitions for
  the same struct name, the first one that matches will be emitted.

* `output_include header.h`: Add `#include <header.h>` to both the generator
  program and the header it generates.  This is mainly useful for including
  `stdint.h` or similar utility definitions in the generated header.  But since
  the generator includes copies of all the struct definitions listed in the
  input file, the header must be included in the generator program as well.

* `output_guard NAME`: Use `NAME` as the name of the include guard for the
  generated header.


### Struct field definitions

The body of a `struct` directive must provide a list of C field declarations to
define the struct body along with a list of field names to be checked against
the host library's definition of the struct.  For simple structs, the name list
is autodetected from the field declarations.  In more complex situations,
attributes can be used for more fine-grained control over the body and the
field names.

Each attribute begins with an `@` and ends at the next space (or at end of
line).  Attributes only affect the current line.

* `@`: Disables automatic field name detection for this line.  No entry will be
  added to the name list, unless a separate annotation adds one explicitly.
* `@name`: Add `name` to the name list, and disable automatic field name
  detection for the line.
* `@name1=name2`: Map `name1` to `name2` in the name list.  When checking field
  compatibility with the host library's version of the struct, field `name1` of
  the host struct will be compared to field `name2` of the defined struct.
* `@@check`: Include this line only in the generator program, for checking
  against host definitions.
* `@@emit`: Include this line only in the generated header.

Examples:

* `@sa_data char sa_data[14];` - The field name is set explicitly to `sa_data`,
  since automated field name detection doesn't work when the name is followed
  by an array specifier (`[14]`).

* `@ unsigned char sin_zero[8];` - The `sin_zero` field is included in the
  generated struct definition, but is not added to the field name list, so it
  won't be checked for compatibility against any field of the host struct.
  This is appropriate because the `sin_zero` field is used only for padding.

* A multi-line definition:
  ```
  @@check @ uint8_t s6_addr_[16];
  @@emit  @ uint8_t s6_addr[16];
  @s6_addr=s6_addr_
  ```
  The field name `s6_addr` is actually a `#define` in glibc, so we can't
  declare a field with that name in the generator program.  Instead we use the
  name `s6_addr_` for compatibility checking.  The `@s6_addr=s6_addr_`
  attribute means that the `s6_addr_` field will be checked against the
  `s6_addr` field of the host struct.  Finally, when the generated header is
  emitted, it will use the original `s6_addr` name, as required by the POSIX
  standard.

'''
from collections import defaultdict, namedtuple
import sys

Directive = namedtuple('Directive', ('kind', 'name', 'args', 'body'))

def parse_directives(path):
    cur_directive = None
    directives = []
    for line in open(path):
        # Skip blank lines and comments.
        stripped = line.strip();
        if stripped == '' or stripped.startswith('#'):
            continue

        if not line.startswith(' '):
            # Non-indented line starts a new directive.
            if cur_directive is not None:
                directives.append(cur_directive)
            parts = line.split(None, 2)
            kind = parts[0] if len(parts) > 0 else ''
            name = parts[1] if len(parts) > 1 else ''
            args = parts[2] if len(parts) > 2 else ''
            cur_directive = Directive(kind, name, args, [])
        else:
            cur_directive.body.append(line)
    if cur_directive is not None:
        directives.append(cur_directive)
    return directives

def parse_struct_body(lines):
    names = []
    check_body = []
    emit_body = []

    for line in lines:
        line = line.strip();
        orig_line = line

        do_auto_name = True
        only_check = False
        only_emit = False
        while line.startswith('@'):
            attr, _, line = line.partition(' ')
            line = line.lstrip()

            assert attr.startswith('@')
            if attr.startswith('@@'):
                a = attr[2:]
                if a == 'check':
                    only_check = True
                elif a == 'emit':
                    only_emit = True
                else:
                    assert False, 'unknown attribute %r in %r' % (a, orig_line)
            else:
                if len(attr) > 1:
                    host, _, my = attr[1:].partition('=')
                    if my == '':
                        my = host
                    names.append((host, my))
                do_auto_name = False

        if do_auto_name:
            assert line.endswith(';'), \
                    'missing semicolon in field def %r' % orig_line
            name = line.rstrip(';').split()[-1]
            assert name.isidentifier(), \
                    'bad autodetected name %r in field def %r' % (name, orig_line)
            names.append((name, name))

        assert not (only_check and only_emit), \
                "can't combine @@check and @@emit (in %r)" % orig_line

        if only_check:
            check_body.append(line)
        elif only_emit:
            emit_body.append(line)
        else:
            check_body.append(line)
            emit_body.append(line)

    return names, check_body, emit_body

def main():
    input_path, = sys.argv[1:]
    directives = parse_directives(input_path)

    directives_by_kind = defaultdict(list)
    structs_by_name = defaultdict(list)
    for d in directives:
        if d.kind == 'struct':
            structs_by_name[d.name].append(d)
        else:
            directives_by_kind[d.kind].append(d)

    print('#include <stdlib.h>')
    print('#include <stdio.h>')
    print('#include <stddef.h>')
    for d in directives_by_kind['include']:
        print('#include <%s>' % d.name)
    for d in directives_by_kind['output_include']:
        print('#include <%s>' % d.name)

    print('#ifdef VERBOSE')
    print('#  define COMPARE(a, b) \\')
    print('    (printf("%s = %u, %s = %d, equal? %d\\n", #a, (int)(a), #b, (int)(b), (a) == (b)), \\')
    print('      (a) == (b))')
    print('#else')
    print('#  define COMPARE(a, b) ((a) == (b))')
    print('#endif')

    for name, ds in structs_by_name.items():
        for i, d in enumerate(ds):
            names, check_lines, emit_lines = parse_struct_body(d.body)
            print('struct my_%s_%d {' % (name, i))
            for line in check_lines:
                print('  ' + line)
            print('};')

            struct_def_lines = ['struct %s {' % name] + emit_lines + ['};']
            print('const char* my_%s_%d_def =' % (name, i))
            for l in struct_def_lines:
                escaped = l \
                        .replace('\\', '\\\\') \
                        .replace('\n', '\\n') \
                        .replace('\"', '\\"')
                print('  "%s\\n"' % escaped)
            print('  ;')

            print('int struct_try_%s_%d() {' % (name, i))
            print('  struct %s host;' % name)
            print('  struct my_%s_%d my;' % (name, i))
            print('  return COMPARE(sizeof(host), sizeof(my))')
            for nh, nm in names:
                print('    && COMPARE(sizeof(host.%s), sizeof(my.%s))' % (nh, nm))
                print('    && COMPARE(offsetof(struct %s, %s), offsetof(struct my_%s_%d, %s))' %
                        (name, nh, name, i, nm))
            print('    ;')
            print('}')

        print('void struct_%s() {' % name)
        for i in range(len(ds)):
            print('  if (struct_try_%s_%d()) {' % (name, i))
            print('    puts(my_%s_%d_def);' % (name, i))
            print('    return;')
            print('  }')
        print('  fputs("error: found no matching definition for %s\\n", stderr);' % name)
        print('  exit(1);')
        print('}')

    print('#define LITERAL_SUFFIX(x) \\')
    print('  (sizeof((x)) == sizeof(long long) ? "ll" : \\')
    print('  sizeof((x)) == sizeof(long) ? "l" : \\')
    print('  "")')

    print('int main() {')

    guard = None
    for d in directives_by_kind['output_guard']:
        guard = d.name
    assert guard, 'missing required output_guard directive'

    print('  puts("#ifndef %s");' % guard)
    print('  puts("#define %s");' % guard)

    for d in directives_by_kind['output_include']:
        print('  puts("#include <%s>");' % d.name)

    for d in directives_by_kind['define']:
        print('  printf("#define {name} %lld%s\\n", '
            '(long long){name}, LITERAL_SUFFIX({name}));'
                .format(name=d.name))

    for name in structs_by_name.keys():
        print('  struct_%s();' % name)

    print('  puts("#endif");')

    print('  return 0;')
    print('}')
            
if __name__ == '__main__':
    main()
