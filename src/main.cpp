#include <sys/io.h>
#include "efi-cpp.h"
#include "list.h"
#include "basic-string.h"

void qemu_exit(UINT8 status) {
    outb(status, 0xf4);
}

EFI_STATUS read_dir_entry(EFI_FILE_HANDLE dir_handle, EFI_FILE_INFO *out, UINTN max_size) {
    UINTN read_size = max_size;
    EFI_STATUS efi_status = uefi_call(dir_handle->Read, 3, dir_handle, &read_size, out);
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
    return (StrCmp(name, L(".")) && StrCmp(name, L("..")));
}

CHAR16 *cat_alloc_triple(const CHAR16 *s1, const CHAR16 *s2, const CHAR16 *s3) {
    UINTN total_len = StrLen(s1) + StrLen(s2) + StrLen(s3);
    CHAR16 *name_buffer = (CHAR16*)AllocatePool(total_len + 1);
    StrCpy(name_buffer, s1);
    StrCat(name_buffer, s2);
    StrCat(name_buffer, s3);
    return name_buffer;
}

void list_dir(EFI_FILE_HANDLE base_handle, const CHAR16 *dir_name, BOOLEAN recursive) {
    class EFIListData {
    public:
        EFI_FILE_HANDLE base;
        String path;
        EFIListData(EFI_FILE_HANDLE base, const CHAR16 *path) : base(base), path(path) { }
        EFIListData(const EFIListData &other) : base(other.base), path(other.path) { }
    };

    UINTN max_file_name_size = 256;
    EFI_STATUS efi_status;
    UINTN max_info_size = SIZE_OF_EFI_FILE_INFO + max_file_name_size;
    EFI_FILE_INFO *file_info = (EFI_FILE_INFO *)AllocatePool(max_info_size);

    EFIListData *loop_data = new EFIListData(base_handle, dir_name);
    List<EFIListData> dir_list;
    dir_list.append(*loop_data);
    auto list_pos = dir_list.iterator();

    do {
        delete loop_data;
        loop_data = new EFIListData(list_pos->data->base, list_pos->data->path);
        base_handle = loop_data->base;
        dir_name = loop_data->path;
        EFI_FILE_HANDLE dir_handle;
        efi_status = uefi_call(
            base_handle->Open, 5, base_handle, &dir_handle, dir_name, EFI_FILE_MODE_READ, 0);
        if (efi_status != EFI_SUCCESS) {
            Print(L("Could not open %s, %d\n"), dir_name, efi_status);
            list_pos = list_pos->next;
        } else {
            do {
                ZeroMem(file_info, max_info_size);
                efi_status = read_dir_entry(dir_handle, file_info, max_info_size);
                if (efi_status != EFI_SUCCESS) {
                    Print(L(" - Couldn't read entry\n"));
                } else if (file_info->Size && should_list(file_info)) {
                    Print(L("%s%s\n"), dir_name, file_info->FileName);
                    if (recursive && is_dir(file_info)) {
                        CHAR16 *name_buffer = cat_alloc_triple(dir_name, file_info->FileName, L("\\"));
                        EFIListData next_data(dir_handle, name_buffer);
                        dir_list.append(next_data);
                        FreePool(name_buffer);
                    }
                }
            } while(efi_status == EFI_SUCCESS && file_info->Size);
        }
    list_pos = list_pos->next;
    } while(list_pos);
    delete loop_data;
    FreePool(file_info);
}

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