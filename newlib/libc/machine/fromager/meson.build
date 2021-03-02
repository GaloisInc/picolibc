# Copyright © 2021 Galois, Inc.  All rights reserved.

srcs_machine = [
  'nanocxx.cpp',

  'stack_protector.c',

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

srcs_machine_builtins = [
	'llvm_intrin.c',
]

foreach target : targets
	value = get_variable('target_' + target)

	builtins_objs = static_library('machine_builtins' + target,
		srcs_machine_builtins,
		pic: false,
		include_directories: inc,
		c_args: value[1] + ['-fno-builtin']).extract_all_objects()

	set_variable('lib_machine' + target,
		static_library('machine' + target,
			srcs_machine,
			objects: builtins_objs,
			pic: false,
			include_directories: inc,
			c_args: value[1]))
endforeach

install_headers(inc_headers_machine)
