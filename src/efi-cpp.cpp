#include "efi-cpp.h"

void *operator new(unsigned long size) {
    return AllocatePool(size);
}

void operator delete(void *ptr, unsigned long size) {
    FreePool(ptr);
}