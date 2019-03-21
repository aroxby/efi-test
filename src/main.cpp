#include "efi-cpp.h"
#include "dir.h"
#include "qemu.h"

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
    EFI_HANDLE_PROTOCOL handle_protocol;
    EFI_STATUS efi_status;
    EFI_LOADED_IMAGE *li;

    InitializeLib(imageHandle, systemTable);

    handle_protocol = systemTable->BootServices->HandleProtocol;
    efi_status = uefi_call(handle_protocol, 3, imageHandle, &loaded_image_protocol, &li);

    if (efi_status != EFI_SUCCESS ) {
        return EFI_UNSUPPORTED;
    }

    Print(L("Device: %s\n"), DevicePathToStr(DevicePathFromHandle(li->DeviceHandle)));

    EFI_FILE_HANDLE dir_handle = LibOpenRoot(li->DeviceHandle);
    list_dir(dir_handle, L("\\"), TRUE);

    qemu_exit(0);
    Print(L("System halted.  Please manually turn off the system.\n"));
    for(;;) {
        asm("hlt");
    }
}