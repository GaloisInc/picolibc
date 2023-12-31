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
srcs_tinystdio = [
    'asprintf.c',
    'clearerr.c',
    'ecvt_data.c',
    'ecvtf_data.c',
    'ecvt.c',
    'ecvtbuf.c',
    'ecvtf.c',
    'ecvtfbuf.c',
    'fcvtbuf.c',
    'fcvt.c',
    'fcvtf.c',
    'fcvtfbuf.c',
    'gcvt.c',
    'gcvtbuf.c',
    'gcvtf.c',
    'gcvtfbuf.c',
    'fclose.c',
    'fdevopen.c',
    'feof.c',
    'ferror.c',
    'fflush.c',
    'fgetc.c',
    'fgets.c',
    'fileno.c',
    'filestrget.c',
    'filestrput.c',
    'filestrputalloc.c',
    'fprintf.c',
    'fputc.c',
    'fputs.c',
    'fread.c',
    'fscanf.c',
    'fseek.c',
    'ftell.c',
    'fwrite.c',
    'getchar.c',
    'gets.c',
    'perror.c',
    'printf.c',
    'putchar.c',
    'puts.c',
    'scanf.c',
    'setbuf.c',
    'setvbuf.c',
    'snprintf.c',
    'sprintf.c',
    'sprintff.c',
    'sprintfd.c',
    'sscanf.c',
    'strtof.c',
    'strtod.c',
    'strtod_l.c',
    'ungetc.c',
    'vasprintf.c',
    'vfiprintf.c',
    'vfprintf.c',
    'vfprintff.c',
    'vfscanf.c',
    'vfiscanf.c',
    'vfscanff.c',
    'vprintf.c',
    'vscanf.c',
    'vsnprintf.c',
    'vsprintf.c',
]

# exact float/string conversion code
srcs_dtoa_ryu = [
  'dtoa_ryu.c',
  'ftoa_ryu.c',
  'ryu_table.c',
  'atod_ryu.c',
  'atof_ryu.c',
  'ryu_log2pow5.c',
  'ryu_log10.c',
  'ryu_pow5bits.c',
  'ryu_umul128.c',
  'ryu_divpow2.c',
  ]
		
# smaller float/string conversion code
srcs_dtoa_float = [
  'dtoa_engine.c',
  'dtoa_data.c',
  'ftoa_data.c',
  'ftoa_engine.c',
  'atod_engine.c',
  'atof_engine.c',
]

if io_float_exact
  srcs_tinystdio += srcs_dtoa_ryu
else
  srcs_tinystdio += srcs_dtoa_float
endif

if io_long_double and have_long_double and not long_double_equals_double
  srcs_tinystdio += ['strtold.c', 'strtold_l.c']
endif

if atomic_ungetc
  srcs_tinystdio += ['exchange.c', 'compare_exchange.c']
endif

srcs_tinystdio_posix = [
    'fopen.c',
    'fdopen.c',
    'fclose.c',
    'posixio.c',
    'sflags.c'
]

srcs_tinystdio_posix_console = [
  'posixiob.c',
]

if posix_io
  srcs_tinystdio += srcs_tinystdio_posix
endif

if posix_console
  srcs_tinystdio += srcs_tinystdio_posix_console
endif

srcs_tinystdio_flt = [
    'vfprintf.c',
    'vfscanf.c',
]

hdrs_tinystdio = [
    'dtoa_engine.h',
    'ftoa_engine.h',
    'stdio.h',
    'stdio_private.h',
    'xtoa_fast.h',
]

install_headers(
  'stdio.h'
)

srcs_tinystdio_use = []
foreach file : srcs_tinystdio
  s_file = file.split('.')[0] + '.S'
  if file in srcs_machine
    message('libc/tinystdio/' + file + ': machine overrides generic')
  elif s_file in srcs_machine
    message('libc/tinystdio/' + s_file + ': machine overrides generic')
  else
    srcs_tinystdio_use += file
  endif
endforeach

foreach target : targets
	value = get_variable('target_' + target)
	set_variable('lib_tinystdio' + target,
		     static_library('tinystdio' + target,
				    srcs_tinystdio_use,
				    pic: false,
				    dependencies: [global_dep],
				    c_args: value[1]))
endforeach
