EFI_INC=/usr/include/efi
EFI_ARCH_INC=$(EFI_INC)/$(ARCH)
INCLUDE=$(EFI_INC) $(EFI_ARCH_INC)
EFI_CRT=/usr/lib/crt0-efi-$(ARCH).o
CFLAGS=-fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER
CFLAGS:=$(CFLAGS) $(foreach d, $(INCLUDE), -I$d)
LDLIBS=-lgnuefi -lefi
LDSCRIPT=-T /usr/lib/elf_$(ARCH)_efi.lds
LDFLAGS=-nostdlib -znocombreloc -shared -Bsymbolic -L/usr/lib $(EFI_CRT) $(LDLIBS) $(LDSCRIPT)
OBJCOPYSECTS=.text .sdata .data .dynamic .dynsym .rel .rela .reloc
OBJCOPYFLAGS=$(foreach s, $(OBJCOPYSECTS), -j $s) --target=efi-app-$(ARCH)

SRC_DIR=src
SRCS=$(shell find $(SRC_DIR) -name '*.c')
OBJS=$(subst .c,.o,$(SRCS))
TARGET_NAME=efi-test
SO_TARGET=$(TARGET_NAME).so
TARGET=$(TARGET_NAME).efi

OBJCOPY=objcopy

.PHONY: default all tidy clean

default: all

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SO_TARGET): $(OBJS)
	$(LD) $< $(LDFLAGS) -o $@

$(TARGET): $(SO_TARGET)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

tidy:
	rm -f $(OBJS) $(SO_TARGET)

clean: tidy
	rm -rf $(TARGET)