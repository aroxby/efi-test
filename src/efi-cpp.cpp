#include "efi-cpp.h"

typedef UINTN SizeType;

void *do_allocate(SizeType size) {
    void *ptr = AllocatePool(size);
    #if CPP_MEM_AUDIT == 1
        Print(L("Allocate: %llx@%llu\n"), (SizeType)ptr, size);
    #endif
    return ptr;
}

void do_free(void *ptr) {
    #if CPP_MEM_AUDIT == 1
        Print(L("Freepool: %llx\n"), (SizeType)ptr);
    #endif
    return FreePool(ptr);
}

void *operator new(SizeType size) {
    return do_allocate(size);
}

void operator delete(void *ptr, SizeType size) {
    return do_free(ptr);
}

void *operator new[](SizeType size) {
    return do_allocate(size);
}

void operator delete[](void *ptr) {
    return do_free(ptr);
}