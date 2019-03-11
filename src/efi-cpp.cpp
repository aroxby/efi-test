#include "efi-cpp.h"

void *operator new(unsigned long size) {
    void *ptr = AllocatePool(size);
    // Print(L("Alee %016x\n"), ptr);
    return ptr;
}

void operator delete(void *ptr, unsigned long size) {
    // Print(L("Free %016x\n"), ptr);
    FreePool(ptr);
}

void *operator new[](unsigned long size) {
    void *ptr = AllocatePool(size);
    // Print(L("Alee %016x\n"), ptr);
    return ptr;
}

void operator delete[](void *ptr) {
    // Print(L("Free %016x\n"), ptr);
    FreePool(ptr);
}