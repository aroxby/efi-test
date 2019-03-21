#include <sys/io.h>

void qemu_exit(unsigned char status) {
    outb(status, 0xf4);
}