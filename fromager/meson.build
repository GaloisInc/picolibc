# Copyright © 2021 Galois, Inc.  All rights reserved.

link_staticlib_bitcode = files('link_staticlib_bitcode.sh')

foreach target : targets
  libc = get_variable('lib_c' + target)

  libc_bitcode = custom_target('libc_bitcode' + target,
    build_by_default: true,
    input: libc,
    output: 'libc.bc',
    command: [link_staticlib_bitcode, '@INPUT@', '@OUTPUT@'],
    install: true,
    install_dir: instdir)

  libm = get_variable('lib_m' + target)

  libm_bitcode = custom_target('libm_bitcode' + target,
    build_by_default: true,
    input: libm,
    output: 'libm.bc',
    command: [link_staticlib_bitcode, '@INPUT@', '@OUTPUT@'],
    install: true,
    install_dir: instdir)
endforeach

