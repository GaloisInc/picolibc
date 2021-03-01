# Copyright © 2021 Galois, Inc.  All rights reserved.

srcs_machine = [
  'llvm_intrin.c',

  # Replace all the malloc implementation functions.
  'malloc-calloc.c',
  'malloc-cfree.c',
  'malloc-free.c',
  'malloc-mallinfo.c',
  'malloc-malloc.c',
  'malloc-malloc_stats.c',
  'malloc-malloc_usable_size.c',
  'malloc-mallopt.c',
  'malloc-memalign.c',
  'malloc-pvalloc.c',
  'malloc-realloc.c',
  'malloc-valloc.c',
]

inc_headers_machine = [
  'fromager.h',
]

foreach target : targets
	value = get_variable('target_' + target)
	set_variable('lib_machine' + target,
		static_library('machine' + target,
			srcs_machine,
			pic: false,
			include_directories: inc,
			c_args: value[1]))
endforeach

install_headers(inc_headers_machine)
