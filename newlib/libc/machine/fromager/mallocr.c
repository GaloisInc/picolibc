/*
 * Implementation of `malloc` using Fromager/Cheesecloth intrinsics.
 *
 * The functions are split into separate objects ...TODO
 * Not all functions are currently implemented, so some of those objects will
 * be empty, and anything that calls the corresponding function will get a
 * linker error.
 */
#include <stdint.h>
#include <stddef.h>
#include <fromager.h>

// Allocate `size` bytes of memory.
char* __cc_malloc(size_t size);
// Free the allocation starting at `ptr`.
void __cc_free(char* ptr);

// Let the prover arbitrarily choose a word to poison in the range `start <=
// ptr < end`.  The prover returns `NULL` to indicate that nothing should be
// poisoned.
uintptr_t* __cc_advise_poison(char* start, char* end);

// Write `val` to `*ptr` and poison `*ptr`.  If `*ptr` is already poisoned, the
// trace is invalid.
void __cc_write_and_poison(uintptr_t* ptr, uintptr_t val);

#ifdef DEFINE_MALLOC
// Allocate a block of `size` bytes.
char* malloc_internal(size_t size) {
    char* ptr = __cc_malloc(size);

    // Compute and validate the size of the allocation provided by the prover.
    uintptr_t addr = (uintptr_t)ptr;
    size_t region_size = 1ull << ((addr >> 58) & 63);
    // The allocated region must have space for `size` bytes, plus an
    // additional word for metadata.
    __cc_valid_if(region_size >= size + sizeof(uintptr_t),
        "allocated region size is too small");
    __cc_valid_if(addr % region_size == 0,
        "allocated address is misaligned for its region size");
    // Note that `region_size` is always a power of two and is at least the
    // word size, so the address must be a multiple of the word size.

    // Write 1 (allocated) to the metadata field, and poison it to prevent
    // tampering.  This will make the trace invalid if the metadata word is
    // already poisoned (this happens if the prover tries to return the same
    // region for two separate allocations).
    uintptr_t* metadata = (uintptr_t*)(ptr + region_size - sizeof(uintptr_t));
    __cc_write_and_poison(metadata, 1);

    // Choose a word to poison in the range `ptr .. metadata`.
    uintptr_t* poison = __cc_advise_poison(ptr + size, (char*)metadata);
    if (poison != NULL) {
        // The poisoned address must be well-aligned.
        __cc_valid_if((uintptr_t)poison % sizeof(uintptr_t) == 0,
            "poison address is not word-aligned");
        // The poisoned address must be in the unused space at the end of the
        // region.
        __cc_valid_if(ptr + size <= (char*)poison,
            "poisoned word overlaps usable allocation");
        __cc_valid_if(poison < metadata,
            "poisoned word overlaps allocation metadata");
        __cc_write_and_poison(poison, 0);
    }

    return ptr;
}

void* malloc(size_t size) {
    return (void*)malloc_internal(size);
}
#endif

#ifdef DEFINE_FREE
void free_internal(char* ptr) {
    if (ptr == NULL) {
        return;
    }

    // Get the allocation size.
    uintptr_t log_region_size = (uintptr_t)ptr >> 58;
    uintptr_t region_size = 1ull << log_region_size;

    // Ensure `ptr` points to the start of a region.
    __cc_bug_if((uintptr_t)ptr % region_size != 0,
        "freed pointer not the start of a region");

    // Write to `*ptr`.  This memory access lets us catch double-free and
    // free-before-alloc by turning them into use-after-free and
    // use-before-alloc bugs, which we catch by other means.
    (*ptr) = 0;

    // We free only after the write, so the interpreter's fine-grained
    // allocation tracking doesn't flag it as a use-after-free.
    __cc_free(ptr);

    // Choose an address to poison.
    uintptr_t* metadata = (uintptr_t*)(ptr + region_size - sizeof(uintptr_t));
    uintptr_t* poison = __cc_advise_poison(ptr, (char*)metadata);
    if (poison != NULL) {
        // The poisoned address must be well-aligned.
        __cc_valid_if((uintptr_t)poison % sizeof(uintptr_t) == 0,
            "poison address is not word-aligned");
        // The pointer must be somewhere within the freed region.
        __cc_valid_if(ptr <= (char*)poison,
            "poisoned word is before the freed region");
        __cc_valid_if(poison < metadata,
            "poisoned word overlaps allocation metadata");
        __cc_write_and_poison(poison, 0);
    }
}

void free(void* ptr) {
    free_internal((char*)ptr);
}
#endif

#ifdef DEFINE_MEMALIGN
int posix_memalign(void **memptr, size_t alignment, size_t size) {
    // `malloc(N)` always returns a pointer that is aligned to the next power
    // of two >= `N`.
    size_t adj_size = size;
    if (alignment > size) {
        size = alignment;
    }
    *memptr = malloc(size);
    return 0;
}
#endif
