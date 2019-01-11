FROM ubuntu:bionic

# Install massive packages in a separate layer
RUN apt-get update && apt-get install -y \
    build-essential \
    qemu-system-x86

RUN apt-get update && apt-get install -y \
    dosfstools \
    gnu-efi \
    mtools \
    ovmf

ENV ARCH=x86_64
ADD . /src
WORKDIR /src
RUN make

RUN mkfs.fat -CF 32 efi.img 65536
RUN mmd -i efi.img ::efi && \
    mmd -i efi.img ::efi/boot && \
    mcopy -i efi.img efi-test.efi ::efi/boot/bootx64.efi

CMD qemu-system-x86_64 \
    -L /usr/share/qemu/ -bios OVMF.fd \
    -drive file=efi.img \
    -nographic  \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04