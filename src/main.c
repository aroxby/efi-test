#include <efi.h>
#include <efilib.h>
#include <sys/io.h>

void qemu_exit(int status) {
    outb(status, 0xf4);
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello, world!\n");
    qemu_exit(0);
    Print(L"System halted.  Please manually turn off the system.\n");
    for(;;) {
        asm("hlt");
    }
}