#include "efi-cpp.h"
#include "list.h"
#include "basic-string.h"

template <UINTN max_name_length>
union NamedFileInfo {
private:
    EFI_FILE_INFO info;
    // FIXME: This size is more than we want.
    // sizeof(EFI_FILE_INFO) gives the size of structure including FileName and padding.
    // It would be better to use offsetof(FileName) but it is not a compile time constant
    CHAR16 name[max_name_length + sizeof(EFI_FILE_INFO)];
public:
    operator EFI_FILE_INFO*() {
        return &info;
    }
};

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
    CHAR16 *name_buffer = new CHAR16[total_len + 1];
    StrCpy(name_buffer, s1);
    StrCat(name_buffer, s2);
    StrCat(name_buffer, s3);
    return name_buffer;
}

void list_dir(EFI_FILE_HANDLE base_handle, const CHAR16 *dir_name, BOOLEAN recursive) {
    struct EFIListData {
        EFI_FILE_HANDLE base;
        String path;
    };

    EFI_STATUS efi_status;
    const static UINTN max_file_name_size = 256;
    NamedFileInfo<max_file_name_size> named_info;
    const static UINTN full_info_size = sizeof(named_info);
    EFI_FILE_INFO *file_info = named_info;

    List<EFIListData> dir_list;
    dir_list.append({base_handle, dir_name});
    auto list_pos = dir_list.iterator();

    do {
        base_handle = list_pos->data->base;
        dir_name = list_pos->data->path;
        EFI_FILE_HANDLE dir_handle;
        efi_status = uefi_call(
            base_handle->Open, 5, base_handle, &dir_handle, dir_name, EFI_FILE_MODE_READ, 0);
        if (efi_status != EFI_SUCCESS) {
            Print(L("Could not open %s, %d\n"), dir_name, efi_status);
            list_pos = list_pos->next;
        } else {
            do {
                ZeroMem(&named_info, full_info_size);
                efi_status = read_dir_entry(dir_handle, file_info, full_info_size);
                if (efi_status != EFI_SUCCESS) {
                    Print(L(" - Couldn't read entry\n"));
                } else if (file_info->Size && should_list(file_info)) {
                    Print(L("%s%s\n"), dir_name, file_info->FileName);
                    if (recursive && is_dir(file_info)) {
                        CHAR16 *name_buffer = cat_alloc_triple(dir_name, file_info->FileName, L("\\"));
                        dir_list.append({dir_handle, name_buffer});
                        delete[] name_buffer;
                    }
                }
            } while(efi_status == EFI_SUCCESS && file_info->Size);
        }
    list_pos = list_pos->next;
    } while(list_pos);
}