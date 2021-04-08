#ifndef FROMAGER_H
#define FROMAGER_H

#ifdef __cplusplus
extern "C" {
#endif

// Indicate that the current trace is invalid.
void __cc_flag_invalid(void);
// Indicate that the current trace has exhibited a bug.
void __cc_flag_bug(void);

// Print a message during evaluation in the MicroRAM interpreter.
void __cc_trace(const char* msg);


// Mark the range from `start` to `end` as valid to access.  This overrides the
// effect of any previous `__cc_mark_invalid` on the range.  All heap memory is
// invalid to access by default; it must be marked with `__cc_access_valid`
// when allocated.
void __cc_access_valid(char* start, char* end);

// Mark the range from `start` to `end` as invalid to access.
void __cc_access_invalid(char* start, char* end);

// Read a word from `*ptr`, bypassing normal memory safety checks.  This read
// will never be considered a memory error.
uintptr_t __cc_read_unchecked(uintptr_t* ptr);

// Write `val` to `*ptr`, bypassing normal memory safety checks.  This write
// will never be considered a memory error.
void __cc_write_unchecked(uintptr_t* ptr, uintptr_t val);


// Assert that the trace is valid only if `cond` is non-zero.  If the trace is
// invalid, `msg` will be printed when running in the MicroRAM interpreter.
static inline void __cc_valid_if(int cond, const char* msg) {
    if (!cond) {
        __cc_flag_invalid();
#ifdef FROMAGER_DEBUG
        __cc_trace(msg);
#endif
    }
}

// Indicate that the program has exhibited a bug if `cond` is non-zero.  If the
// bug is present, `msg` will be printed when running in the MicroRAM
// interpreter.
static inline void __cc_bug_if(int cond, const char* msg) {
    if (cond) {
        __cc_flag_bug();
#ifdef FROMAGER_DEBUG
        __cc_trace(msg);
#endif
    }
}


void* __cc_heap_snapshot(size_t* len) __attribute__((noinline));
void* __cc_malloc_heap_start() __attribute__((noinline));
void* __cc_malloc_heap_end() __attribute__((noinline));
void __cc_malloc_set_heap_end(void* new_end) __attribute__((noinline));
void __cc_malloc_init_from_snapshot(void* addr, size_t len) __attribute__((noinline));

void __cc_trace_exec(
        const char* name,
        uintptr_t arg0,
        uintptr_t arg1,
        uintptr_t arg2,
        uintptr_t arg3,
        uintptr_t arg4,
        uintptr_t arg5,
        uintptr_t arg6,
        uintptr_t arg7);

#ifdef __cplusplus
}
#endif

#endif // FROMAGER_H
