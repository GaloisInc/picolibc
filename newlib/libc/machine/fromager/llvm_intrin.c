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

uint32_t __llvm__bswap__i32(uint32_t x) {
    volatile uint32_t x0 = x;
    uint8_t a = x0 >> 0;
    uint8_t b = x >> 8;
    uint8_t c = x >> 16;
    uint8_t d = x >> 24;
    return d | (c << 8) | (b << 16) | (a << 24);
}

uint32_t __llvm__ctpop__i32(uint32_t x) {
    x = (x & 0x55555555) + ((x & 0xaaaaaaaa) >> 1);
    x = (x & 0x33333333) + ((x & 0xcccccccc) >> 2);
    x = (x & 0x0f0f0f0f) + ((x & 0xf0f0f0f0) >> 4);
    x = (x & 0x00ff00ff) + ((x & 0xff00ff00) >> 8);
    x = (x & 0x0000ffff) + ((x & 0xffff0000) >> 16);
    return x;
}

int32_t __cc_sdiv_i32_i32(int32_t x, int32_t y) {
    int32_t sign = 1;
    if (x < 0) {
        x = -x;
        sign = -sign;
    }
    if (y < 0) {
        y = -y;
        sign = -sign;
    }
    return (int32_t)((uint32_t)x / (uint32_t)y) * sign;
}

int32_t __cc_srem_i32_i32(int32_t x, int32_t y) {
    int32_t sign = 1;
    if (x < 0) {
        x = -x;
        sign = -sign;
    }
    if (y < 0) {
        y = -y;
        sign = -sign;
    }
    return (int32_t)((uint32_t)x % (uint32_t)y) * sign;
}

int64_t __cc_sdiv_i64_i64(int64_t x, int64_t y) {
    int64_t sign = 1;
    if (x < 0) {
        x = -x;
        sign = -sign;
    }
    if (y < 0) {
        y = -y;
        sign = -sign;
    }
    return (int64_t)((uint64_t)x / (uint64_t)y) * sign;
}

int64_t __cc_srem_i64_i64(int64_t x, int64_t y) {
    int64_t sign = 1;
    if (x < 0) {
        x = -x;
        sign = -sign;
    }
    if (y < 0) {
        y = -y;
        sign = -sign;
    }
    return (int64_t)((uint64_t)x % (uint64_t)y) * sign;
}
