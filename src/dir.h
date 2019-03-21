#ifndef _INC_DIR_H
#define _INC_DIR_H

#include "efi-cpp.h"

void list_dir(EFI_FILE_HANDLE base_handle, const CHAR16 *dir_name, BOOLEAN recursive);

#endif//_INC_DIR_H