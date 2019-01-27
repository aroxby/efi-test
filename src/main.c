#include <efi.h>
#include <efilib.h>
#include <sys/io.h>

void qemu_exit(UINT8 status) {
    outb(status, 0xf4);
}

void print_file_info(EFI_FILE_INFO *info) {
    if (info) {
        Print(L"Size: %d\n", info->FileSize);
        Print(L"Attributes: 0x%x\n", info->Attribute);
        if (info->Size > SIZE_OF_EFI_FILE_INFO) {
            Print(L"Name: %s\n", info->FileName);
        } else {
            UINTN ch = info->FileName[0];
            Print(L"No name (%d/%d)(%d)\n", info->Size, SIZE_OF_EFI_FILE_INFO + 1, ch);
        }
    } else {
        Print(L"Bad info\n");
    }
}

EFI_STATUS read_dir_entry(EFI_FILE_HANDLE dir_handle, EFI_FILE_INFO *out, UINTN max_size) {
    UINTN read_size = max_size;
    EFI_STATUS efi_status = uefi_call_wrapper(dir_handle->Read, 3, dir_handle, &read_size, out);
    if (!read_size) {
        out->Size = 0;
    }
    return efi_status;
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

    UINTN max_file_name_size = 256;
    UINTN max_info_size = SIZE_OF_EFI_FILE_INFO + max_file_name_size;
    EFI_FILE_HANDLE dir_handle = LibOpenRoot(li->DeviceHandle);
    EFI_FILE_INFO *file_info = AllocatePool(max_info_size);
    do {
        ZeroMem(file_info, max_info_size);
        efi_status = read_dir_entry(dir_handle, file_info, max_info_size);
        file_info->FileName[255] = 0;
        if (efi_status != EFI_SUCCESS ) {
            Print(L" - Couldn't read entry\n");
        } else if (file_info->Size) {
            print_file_info(file_info);
        }
    } while(efi_status == EFI_SUCCESS && file_info->Size);

    qemu_exit(0);
    Print(L"System halted.  Please manually turn off the system.\n");
    for(;;) {
        asm("hlt");
    }
}