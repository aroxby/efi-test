#ifndef _INC_EFI_CPP_H
#define _INC_EFI_CPP_H

#define uefi_call(func, ...) uefi_call_wrapper((void*)func, __VA_ARGS__)
#define L(s) ((CHAR16*)(L##s))

#include <cstdint>

extern "C" {
    #include <efi.h>
    #include <efilib.h>
}

#endif//_INC_EFI_CPP_H