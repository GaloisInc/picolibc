#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2019 Keith Packard
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
srcs_time = [
    'asctime.c',
    'asctime_r.c',
    'clock.c',
    'ctime.c',
    'ctime_r.c',
    'difftime.c',
    'gettzinfo.c',
    'gmtime.c',
    'gmtime_r.c',
    'lcltime.c',
    'lcltime_r.c',
    'lcltime_buf.c',
    'mktime.c',
    'month_lengths.c',
    'strftime.c',
    'strptime.c',
    'time.c',
    'tzcalc_limits.c',
    'tzlock.c',
    'tzset.c',
    'tzvars.c',
]
if not tinystdio
  srcs_time += 'wcsftime.c'
endif

hdrs_time = [
    'local.h',
]

srcs_time_use = []
foreach file : srcs_time
  s_file = file.split('.')[0] + '.S'
  if file in srcs_machine
    message('libc/time/' + file + ': machine overrides generic')
  elif s_file in srcs_machine
    message('libc/time/' + s_file + ': machine overrides generic')
  else
    srcs_time_use += file
  endif
endforeach

foreach target : targets
	value = get_variable('target_' + target)
	set_variable('lib_time' + target,
		static_library('time' + target,
			srcs_time_use,
			pic: false,
			dependencies: [global_dep],
			c_args: value[1]))
endforeach
