# Copyright © 2021 Galois, Inc.  All rights reserved.

srcs_machine = [
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
