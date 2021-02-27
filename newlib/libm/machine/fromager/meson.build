# Copyright © 2021 Galois, Inc.  All rights reserved.

srcs_libm_machine_real = [
]

srcs_libm_machine = srcs_libm_machine_real + [
]

foreach target : targets
	value = get_variable('target_' + target)
	set_variable('lib_machine' + target,
		static_library('machine' + target,
			srcs_libm_machine_real,
			pic: false,
			include_directories: math_inc,
			c_args: value[1] + arg_fnobuiltin))
endforeach
