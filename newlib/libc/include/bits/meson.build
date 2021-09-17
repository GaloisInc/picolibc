
inc_headers = [
]

install_headers(inc_headers,
		subdir:'bits'
		)


generated_names = [
  'fcntl',
  'mman',
  'netinet_in',
  'socket'
]

prog_python3 = import('python').find_installation('python3')

generated_headers = []
foreach name : generated_names
  generator_src = custom_target('mirror-generator-src-bits-' + name,
    input: [name + '.in'],
    output: [name + '-generator.c'],
    command: [prog_python3, '@SOURCE_ROOT@/fromager/gen_mirror_header.py', '@INPUT@'],
    capture: true)

  generator_exe = executable('mirror-generator-bits-' + name, generator_src,
    native: true)

  generated = custom_target('mirror-header-bits-' + name,
    output: [name + '.h'],
    command: [generator_exe],
    capture: true,
    # `install_headers` doesn't accept `custom_target`s, so we have to install
    # each generated header directly.
    install: true,
    install_dir: get_option('includedir') + '/bits')

  generated_headers += [generated]
endforeach

bits_generated = declare_dependency(
  sources: generated_headers)
