#include <efi.h>
#include <efilib.h>
#include <sys/io.h>

void qemu_exit(UINT8 status) {
    outb(status, 0xf4);
}

void print_file_info(EFI_FILE_INFO *info) {
    if(info) {
        Print(L"Size: %d\n", info->FileSize);
        Print(L"Attributes: 0x%x\n", info->Attribute);
        if(info->Size > SIZE_OF_EFI_FILE_INFO) {
            Print(L"Name: %s\n", info->FileName);
        } else {
            UINTN ch = info->FileName[0];
            Print(L"No name (%d/%d)(%d)\n", info->Size, SIZE_OF_EFI_FILE_INFO + 1, ch);
        }
    } else {
        Print(L"Bad info\n");
    }
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
    EFI_HANDLE_PROTOCOL handle_protocol;
    EFI_STATUS efi_status;
    EFI_LOADED_IMAGE *li;

    InitializeLib(imageHandle, systemTable);

    handle_protocol = systemTable->BootServices->HandleProtocol;
    efi_status = uefi_call_wrapper(handle_protocol, 3, imageHandle, &loaded_image_protocol, &li);

    if (efi_status != EFI_SUCCESS ) {
        return EFI_UNSUPPORTED;
    }

    Print(L"Device: %s\n", DevicePathToStr(DevicePathFromHandle(li->DeviceHandle)));
    
    UINTN readSize = 512;
    EFI_FILE_HANDLE dir_handle = LibOpenRoot(li->DeviceHandle);
    EFI_FILE_INFO *dir_info = LibFileInfo(dir_handle);
    EFI_FILE_INFO *file_info = AllocateZeroPool(readSize);
    efi_status = uefi_call_wrapper(dir_handle->Read, 3, dir_handle, &readSize, file_info);
    
    print_file_info(dir_info);
    if (efi_status != EFI_SUCCESS ) {
        Print(L"Couldn't read dir\n");
    } else {
        print_file_info(file_info);
    }

    qemu_exit(0);
    Print(L"System halted.  Please manually turn off the system.\n");
    for(;;) {
        asm("hlt");
    }
}