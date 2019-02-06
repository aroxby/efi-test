EFI_INC=/usr/include/efi
EFI_ARCH_INC=$(EFI_INC)/$(ARCH)
INCLUDE=$(EFI_INC) $(EFI_ARCH_INC)
EFI_CRT=/usr/lib/crt0-efi-$(ARCH).o
CFLAGS=-fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER
CFLAGS:=$(CFLAGS) -fpermissive -w
CFLAGS:=$(CFLAGS) $(foreach d, $(INCLUDE), -I$d) -Werror=implicit-function-declaration
LDLIBS=-lgnuefi -lefi
LDSCRIPT=-T /usr/lib/elf_$(ARCH)_efi.lds
LDFLAGS=-nostdlib -znocombreloc -shared -Bsymbolic -L/usr/lib $(EFI_CRT) $(LDLIBS) $(LDSCRIPT)
OBJCOPYSECTS=.text .sdata .data .dynamic .dynsym .rel .rela .reloc
OBJCOPYFLAGS=$(foreach s, $(OBJCOPYSECTS), -j $s) --target=efi-app-$(ARCH)

SRC_DIR=src
SRCS=$(shell find $(SRC_DIR) -name '*.cpp')
OBJS=$(subst .cpp,.o,$(SRCS))
TARGET_NAME=efi-test
SO_TARGET=$(TARGET_NAME).so
TARGET=$(TARGET_NAME).efi
DISK=$(TARGET_NAME).fat

OBJCOPY=objcopy

.PHONY: default all disk tidy clean

default: all

all: disk

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(SO_TARGET): $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

$(TARGET): $(SO_TARGET)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(DISK): $(TARGET)
	mkfs.fat -CF 32 $@ 65536
	mmd -i $@ ::efi && \
	mmd -i $@ ::efi/boot && \
	mcopy -i $@ $< ::efi/boot/bootx64.efi

disk: $(DISK)

tidy:
	rm -f $(OBJS) $(SO_TARGET)

clean: tidy
	rm -rf $(TARGET) $(DISK)