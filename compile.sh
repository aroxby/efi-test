#!/bin/sh

gcc src/main.c                             \
      -c                                 \
      -fno-stack-protector               \
      -fpic                              \
      -fshort-wchar                      \
      -mno-red-zone                      \
      -I/usr/include/efi/ -I/usr/include/efi/${ARCH} \
      -DEFI_FUNCTION_WRAPPER             \
      -o main.o && \
ld main.o                         \
     /usr/lib/crt0-efi-${ARCH}.o     \
     -nostdlib                      \
     -znocombreloc                  \
     -T /usr/lib/elf_${ARCH}_efi.lds \
     -shared                        \
     -Bsymbolic                     \
     -L/usr/lib \
     -lgnuefi                 \
     -lefi                    \
     -o main.so && \
objcopy -j .text                \
          -j .sdata               \
          -j .data                \
          -j .dynamic             \
          -j .dynsym              \
          -j .rel                 \
          -j .rela                \
          -j .reloc               \
          --target=efi-app-${ARCH} \
          main.so                 \
          main.efi