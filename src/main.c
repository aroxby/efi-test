#include <efi.h>
#include <efilib.h>
#include <sys/io.h>
#include "list.h"

void qemu_exit(UINT8 status) {
    outb(status, 0xf4);
}

EFI_STATUS read_dir_entry(EFI_FILE_HANDLE dir_handle, EFI_FILE_INFO *out, UINTN max_size) {
    UINTN read_size = max_size;
    EFI_STATUS efi_status = uefi_call_wrapper(dir_handle->Read, 3, dir_handle, &read_size, out);
    if (!read_size) {
        out->Size = 0;
    }
    return efi_status;
}

BOOLEAN is_dir(EFI_FILE_INFO *info) {
    UINTN dir_mask = EFI_FILE_DIRECTORY;
    return (info->Attribute & dir_mask) != 0;
}

BOOLEAN should_list(EFI_FILE_INFO *info) {
    const CHAR16 *name = info->FileName;
    return (StrCmp(name, L".") && StrCmp(name, L".."));
}

void dump_list(ListNode *lst) {
    Print(L"(%x)>%x\n", lst?lst->prev:NULL, lst);
    while(lst) {
        lst = lst->next;
        Print(L">%x\n", lst);
    }
}

void list_dir(EFI_FILE_HANDLE base_handle, const CHAR16 *dir_name, BOOLEAN recursive) {
    typedef struct {
        EFI_FILE_HANDLE base;
        const CHAR16 *path;
    } EFIListData;

    UINTN max_file_name_size = 256;
    EFI_STATUS efi_status;
    UINTN max_info_size = SIZE_OF_EFI_FILE_INFO + max_file_name_size;
    EFI_FILE_INFO *file_info = AllocatePool(max_info_size);

    EFIListData loop_data = {base_handle, dir_name};
    ListNode *dir_list = createDataNode(EFIListData, loop_data);

    do {
        // TODO: getNodeData should output via formal parameter
        loop_data = getNodeData(EFIListData, dir_list);
        base_handle = loop_data.base;
        dir_name = loop_data.path;
        EFI_FILE_HANDLE dir_handle;
        efi_status = uefi_call_wrapper(
            loop_data.base->Open, 5, base_handle, &dir_handle, dir_name, EFI_FILE_MODE_READ, 0);
        if (efi_status != EFI_SUCCESS) {
            Print(L"Could not open %s, %d\n", loop_data.path, efi_status);
            // FIXME: Flow is a little confusing
            dir_list = dir_list->next;
            continue;
        }
        do {
            ZeroMem(file_info, max_info_size);
            efi_status = read_dir_entry(dir_handle, file_info, max_info_size);
            if (efi_status != EFI_SUCCESS) {
                Print(L" - Couldn't read entry\n");
            } else if (file_info->Size && should_list(file_info)) {
                Print(L"%s%s\n", dir_name, file_info->FileName);
                if (recursive && is_dir(file_info)) {
                    // FIXME: This will need freed
                    CHAR16 *name_buffer = AllocatePool(max_file_name_size);
                    // FIXME: Possible overflow
                    StrCpy(name_buffer, dir_name);
                    StrCat(name_buffer, file_info->FileName);
                    StrCat(name_buffer, L"\\");
                    loop_data.base = dir_handle;
                    loop_data.path = name_buffer;
                    ListNode *new = insertData(dir_list, EFIListData, loop_data);
                }
            }
        } while(efi_status == EFI_SUCCESS && file_info->Size);
    dir_list = dir_list->next;
    } while(dir_list);
    FreePool(file_info);
    // TODO: The list still needs freed
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

    EFI_FILE_HANDLE dir_handle = LibOpenRoot(li->DeviceHandle);
    list_dir(dir_handle, L"\\", TRUE);

    qemu_exit(0);
    Print(L"System halted.  Please manually turn off the system.\n");
    for(;;) {
        asm("hlt");
    }
}