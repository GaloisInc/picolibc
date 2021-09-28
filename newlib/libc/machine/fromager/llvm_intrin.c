#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void __llvm__memcpy__p0i8__p0i8__i64(uint8_t *dest, const uint8_t *src, uint64_t len) {
    uint8_t* dest_end = dest + len;
    for (; dest != dest_end; ++src, ++dest) {
        *dest = *src;
    }
}

void __llvm__memmove__p0i8__p0i8__i64(uint8_t *dest, const uint8_t *src, uint64_t len) {
    if ((uintptr_t)dest <= (uintptr_t)src) {
        uint8_t* dest_end = dest + len;
        for (; dest != dest_end; ++src, ++dest) {
            *dest = *src;
        }
    } else {
        uint8_t* dest_start = dest;
        dest += len;
        src += len;
        while (dest != dest_start) {
            --src;
            --dest;
            *dest = *src;
        }
    }
}

void __llvm__memset__p0i8__i64(uint8_t *dest, uint8_t val, uint64_t len) {
    if (len >= 4 * sizeof(uintptr_t)) {
        uint8_t* dest_end = dest + len;
        // Round `dest` up to the next multiple of the word size.
        uint8_t* prefix = (uint8_t*)
            (((uintptr_t)dest + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1));
        // Round `dest_end` down to the previous multiple of the word size.
        uint8_t* suffix = (uint8_t*)((uintptr_t)dest_end & ~(sizeof(uintptr_t) - 1));

        uintptr_t big_val = 0;
        for (int i = 0; i < sizeof(uintptr_t); ++i) {
            big_val = big_val | (val << (i * 8));
        }

        for (; dest != prefix; ++dest) {
            *dest = val;
        }
        for (; dest != suffix; dest += sizeof(uintptr_t)) {
            *(uintptr_t*)dest = big_val;
        }
        for (; dest != dest_end; ++dest) {
            *dest = val;
        }
    } else {
        uint8_t* dest_end = dest + len;
        for (; dest != dest_end; ++dest) {
            *dest = val;
        }
    }
}


int32_t __bswapsi2(int32_t a);
uint32_t __llvm__bswap__i32(uint32_t x) {
    return __bswapsi2(x);
}

int __popcountsi2(unsigned int a);
uint32_t __llvm__ctpop__i32(uint32_t x) {
  return __popcountsi2(x);
}

int __clzsi2(unsigned int a);
int __clzdi2(unsigned long a);
int __ctzsi2(unsigned int a);
int __ctzdi2(unsigned long a);

uint32_t __llvm__ctlz__i32(uint32_t x, int is_zero_undef) {
  return __clzsi2(x);
}

uint64_t __llvm__ctlz__i64(uint64_t x, int is_zero_undef) {
  return __clzdi2(x);
}

uint32_t __llvm__cttz__i32(uint32_t x, int is_zero_undef) {
  return __ctzsi2(x);
}

uint64_t __llvm__cttz__i64(uint64_t x, int is_zero_undef) {
  return __ctzdi2(x);
}


int __divsi3(int a, int b);
int32_t __cc_sdiv_i32_i32(int32_t x, int32_t y) {
  return __divsi3(x, y);
}

int __modsi3(int a, int b);
int32_t __cc_srem_i32_i32(int32_t x, int32_t y) {
  return __modsi3(x, y);
}

long __divdi3(long a, long b);
int64_t __cc_sdiv_i64_i64(int64_t x, int64_t y) {
  return __divdi3(x, y);
}

long __moddi3(long a, long b);
int64_t __cc_srem_i64_i64(int64_t x, int64_t y) {
  return __moddi3(x, y);
}



// We can't use va_list, so redefine it here.
typedef struct {
  unsigned int gp_offset;
  unsigned int fp_offset;
  void* overflow_arg_area;
  void* reg_save_area;
} __cc_va_list;

// The full discussion behind the implementation of `va_start` is available [here](https://gitlab-ext.galois.com/fromager/cheesecloth/MicroRAM/-/issues/48). `bp` and `offset` are used to compute the location of the first variable argument on the stack. `bp` is the base pointer and `offset` is the offset of the first variable argument relative to the base pointer.
void __cc_va_start(char* raw_list, char* bp, int offset) {
    __cc_va_list* list = (__cc_va_list*) raw_list;

    // Set gp_offset to 999. The value 999 is larger than any legal offset, so that the va_arg code always uses the overflow_arg_area case for argument types that are normally passed in general-purpose registers.
    list->gp_offset = 999;

    // Set fp_offset to 999. Same as above, but for floating-point registers.
    list->fp_offset = 999;

    // Set overflow_arg_area to first variable argument on the stack.
    list->overflow_arg_area = bp + offset;

    // Set reg_save_area to 0xffff_0000, which is an invalid but non-null pointer. This way accessing *(reg_save_area + offset) will be an out-of-bounds memory access for any reasonable offset.
    list->reg_save_area = (void*) 0xffff0000;
}

void __llvm__va_copy(char* dest, char* src) {
    __cc_va_list* dest_list = (__cc_va_list*) dest;
    __cc_va_list* src_list = (__cc_va_list*) src;
    *dest_list = *src_list;
}
