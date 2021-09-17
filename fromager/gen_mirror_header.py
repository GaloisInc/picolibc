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

  Each line should define a single field.  If the field definition is trivial
  (such as `int x;`), then this script will detect the field name
  automatically.  In cases where this fails, the line containing the field
  definition should be prefixed with `@name`, as in `@a int a[10];`, to provide
  the name of the field.  If the line begins with `@` without a name, then any
  field name auto-detected on this line will be ignored.  And if the line
  consists of only `@name` with no subsequent definition, then the field name
  is recorded but nothing is emitted when giving the struct definition.  This
  is useful for complex situations involving anonymous inner structs or unions.

* `output_include header.h`: Add `#include <header.h>` to both the generator
  program and the header it generates.  This is mainly useful for including
  `stdint.h` or similar utility definitions in the generated header.  But since
  the generator includes copies of all the struct definitions listed in the
  input file, the header must be included in the generator program as well.

* `output_guard NAME`: Use `NAME` as the name of the include guard for the
  generated header.
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
    defs = []
    for line in lines:
        line = line.strip();
        orig_line = line

        do_auto_name = True
        while line.startswith('@'):
            attr, _, line = line.partition(' ')
            line = line.lstrip()

            assert attr.startswith('@')
            if len(attr) > 1:
                names.append(attr[1:])
            do_auto_name = False

        if do_auto_name:
            assert line.endswith(';'), \
                    'missing semicolon in field def %r' % orig_line
            name = line.rstrip(';').split()[-1]
            assert name.isidentifier(), \
                    'bad autodetected name %r in field def %r' % (name, orig_line)
            names.append(name)

        defs.append(line)

    return names, defs

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
            names, defs = parse_struct_body(d.body)
            print('struct my_%s_%d {' % (name, i))
            for line in defs:
                print('  ' + line)
            print('};')

            struct_def_lines = ['struct %s {' % name] + defs + ['};']
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
            for n in names:
                print('    && COMPARE(sizeof(host.%s), sizeof(my.%s))' % (n, n))
                print('    && COMPARE(offsetof(struct %s, %s), offsetof(struct my_%s_%d, %s))' %
                        (name, n, name, i, n))
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
