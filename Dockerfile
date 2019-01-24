FROM ubuntu:bionic

RUN apt-get update && apt-get install -y \
    build-essential \
    dosfstools \
    gnu-efi \
    mtools \
    ovmf \
    qemu-system-x86

ENV ARCH=x86_64
ADD . /src
WORKDIR /src
RUN make

CMD qemu-system-x86_64 \
    -L /usr/share/qemu/ -bios OVMF.fd \
    -drive file=efi-test.fat \
    -nographic \
    -device isa-debug-exit,iobase=0xf4,iosize=0x01 || exit $(($?>>1))