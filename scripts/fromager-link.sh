#!/bin/bash
set -e

# Common build script for MicroRAM and native targets.  Typically, it should be
# called as the final step of a larger build script for some program.
#
# The build process is controlled using a variety of environment variables:
# * cc_objects: List of object files (actually LLVM bitcode files) to include
#   in the build.  This can include static libraries (`.a`) as well.  All
#   provided object files will be linked together to build the application.
# * cc_secret_objects: Additional object files (or libraries) that contain
#   prover secrets.  These will be linked in like the files in `cc_objects`,
#   but at a later stage of the build.
# * cc_microram_objects, cc_native_objects: Additional object files (or
#   libraries) to include only for MicroRAM/native builds.  These are linked in
#   at the same time as `cc_objects`.
# * cc_build_dir: A directory for storing intermediate build files.
# * cc_microram_output, cc_native_output: Where to place the final output of
#   the MicroRAM build (a `.ll` file) or the native build (an executable).
# * cc_instrument: If set, function entries are instrumented with calls to
#   `__cc_trace_exec`, recording the name of the function and the arguments it
#   was called with.
# * cc_keep_debug: If set, the output native binary will contain debug info.
#   (By default, debug info is stripped instead.)
#
# This script also takes a single optional command-line argument, `MODE`, which
# can be either `microram`, `native`, or `all` (the default).  This controls
# which targets the program will be compiled for.

if [[ -z "$cc_objects" ]]; then
    echo 'warning: $cc_objects is empty (should contain common object files)'
fi

if [[ -z "$cc_secret_objects" ]]; then
    echo 'warning: $cc_secret_objects is empty (should contain secret object files)'
fi

: ${cc_build_dir:=build}
: ${cc_microram_output:=driver-link.ll}
: ${cc_native_output:=driver}

if [[ -n "$cc_instrument" ]]; then
    instrument_args=--cc-instrument
    # --cc-instrument doesn't add the necessary debug info, causing an error
    # when debug info is not stripped.
    cc_keep_debug=
fi
if [[ -z "$cc_keep_debug" ]]; then
    strip_debug_args=--strip-debug
fi

PICOLIBC_HOME="$(dirname "$0")/.."

unpack_objects() {
    for f in "$@"; do
        case $f in
            --override=*)
                for f in $(unpack_objects "${f#--override=}"); do
                    echo "--override=$f"
                done
                ;;

            *.o|*.bc)
                echo "$f"
                ;;

            *.a)
                local dir="${cc_build_dir}/unpacked_libs/$(basename "$f")"
                rm -rf "$dir/"
                mkdir -p "$dir"
                ( cd "$dir" && ar x /dev/stdin ) <"$f"
                unpack_objects "$dir"/*
                ;;

            *)
                echo "unsupported extension for object file: $f"
                return 1
        esac
    done
}

do_build() {
    local mode="$1"
    echo "build: $mode"

    local extra_link=""
    local extra_post_link=""
    if [[ "$mode" == "microram" ]]; then
        extra_link="$PICOLIBC_HOME/lib/libmachine_syscalls.a"
        extra_post_link="$PICOLIBC_HOME/lib/libmachine_builtins.a"
    elif [[ "$mode" == "native" ]]; then
        extra_link="$PICOLIBC_HOME/lib/libmachine_syscalls_native.a"
    else
        echo "bad mode: $mode"
        return 1
    fi

    local work_dir="$cc_build_dir/link_$mode"
    mkdir -p "$work_dir"

    # Link in the libraries prior to optimizing.  We don't add the driver secrets
    # yet, so the optimizer won't propagate information about them.
    llvm-link${LLVM_SUFFIX} \
        $(unpack_objects $PICOLIBC_HOME/lib/libc.a) \
        $(unpack_objects $PICOLIBC_HOME/lib/libm.a) \
        $(unpack_objects $extra_link) \
        $(unpack_objects $cc_objects) \
        -o "$work_dir/driver-nosecret.bc"

    keep_symbols=main
    keep_symbols=$keep_symbols,__llvm__memcpy__p0i8__p0i8__i64
    keep_symbols=$keep_symbols,__llvm__memmove__p0i8__p0i8__i64
    keep_symbols=$keep_symbols,__llvm__memset__p0i8__i64
    keep_symbols=$keep_symbols,__llvm__bswap__i32
    keep_symbols=$keep_symbols,__llvm__ctpop__i32
    keep_symbols=$keep_symbols,__cc_sdiv_i32_i32
    keep_symbols=$keep_symbols,__cc_srem_i32_i32
    keep_symbols=$keep_symbols,__cc_sdiv_i64_i64
    keep_symbols=$keep_symbols,__cc_srem_i64_i64
    if [[ "$mode" == "native" ]]; then
        # Explicitly preserve some libm symbols.  `globaldce` can't see the connection
        # between `llvm.floor.f64` and `floor` and will wrongly remove the latter.
        keep_symbols=$keep_symbols,ceil,floor,trunc,llrint
        keep_symbols=$keep_symbols,exp,exp2,log,pow
        keep_symbols=$keep_symbols,memcpy,memmove,memset
    fi

    # Optimize, removing unused public symbols
    opt${LLVM_SUFFIX} \
        -load ../llvm-passes/passes.so \
        --internalize --internalize-public-api-list="$keep_symbols" \
        $instrument_args \
        --force-vector-width=1 \
        -O3 --unroll-vectors -O1 \
        $strip_debug_args \
        "$work_dir/driver-nosecret.bc" \
        -o "$work_dir/driver-nosecret-opt.bc"

    # Link the driver code and the secrets.  No more optimizations should be run on
    # the code after this.
    llvm-link${LLVM_SUFFIX} \
        "$work_dir/driver-nosecret-opt.bc" \
        $(unpack_objects $cc_secret_objects) \
        $(unpack_objects $extra_post_link) \
        -o "$work_dir/driver-full.bc"

    opt${LLVM_SUFFIX} \
        "$work_dir/driver-full.bc" \
        $strip_debug_args --globaldce \
        -o "$work_dir/driver-full-nodebug.bc"

    if [[ "$mode" == "microram" ]]; then
        # Disassemble to LLVM's textual IR format
        llvm-dis${LLVM_SUFFIX} "$work_dir/driver-full-nodebug.bc" -o "$cc_microram_output"
        sed -i -e 's/nofree//g' "$cc_microram_output"
    elif [[ "$mode" == "native" ]]; then
        llvm-link${LLVM_SUFFIX} \
            "$work_dir/driver-full-nodebug.bc" \
            $(unpack_objects "$PICOLIBC_HOME/lib/libmachine_native.a") \
            -o "$work_dir/driver-full-native.bc"

        clang++${LLVM_SUFFIX} \
            -o "$cc_native_output" \
            "$work_dir/driver-full-native.bc"
    fi
}

if [[ "$1" == "all" ]] || [[ -z "$1" ]]; then
    do_build microram
    do_build native
else
    do_build "$1"
fi
