
BASE = ..
include $(BASE)/config

# load additional configuration
include config/hw/$(PLATFORM).cfg
include config/sw/$(SOFTWARE).cfg

# config toolchain and other stuff
TOOLCHAIN ?= arm-none-eabi-
CC =$(TOOLCHAIN)gcc
LD =$(TOOLCHAIN)ld
AS =$(TOOLCHAIN)gcc
STRIP =$(TOOLCHAIN)strip
OBJCOPY =$(TOOLCHAIN)objcopy
OBJDUMP =$(TOOLCHAIN)objdump
GDB =$(TOOLCHAIN)gdbcd
MKIMAGE =mkimage

# add our own files
SRC_DIRS += hypervisor hypervisor/guest_config lib guests guests/guest1 guests/guest2
INC_DIRS += hypervisor include lib hypervisor/guest_config

# add base library
INC_DIRS +=../library/include
LIB_DIRS += ../library/build
LIB_FILES += minic

# get file lists from dirs defined by configuration
SRC_FILES = $(foreach dir,$(SRC_DIRS), $(wildcard $(dir)/*.c) )
ASM_FILES = $(foreach dir,$(ASM_DIRS), $(wildcard $(dir)/*.S) )
OBJ_FILES=$(SRC_FILES:.c=.o) $(ASM_FILES:.S=.o)

# add include dirs to flags
SRC_CFLAGS = $(foreach dir,$(INC_DIRS), -I$(dir))
ASM_AFLAGS = $(foreach dir,$(INC_DIRS), -I$(dir))


LFLAGS += $(foreach dir,$(LIB_DIRS), -L$(dir))
LFLAGS += $(foreach file,$(LIB_FILES), -l$(file))


##
# get CFLAGS
CFLAGS += $(SRC_CFLAGS) \
	-Os -g -Wall \
	-nostartfiles -nostdlib -ffreestanding\
	-mno-thumb-interwork\
	-march=armv4
#	-pedantic -Wall -Werror\
#	-O3 -fno-inline -finline-limit=1 --param max-unroll-times=0
#	-Os -g -Wall \

ASFLAGS += $(ASM_AFLAGS) -g -c

LFLAGS += $(shell $(CC) -print-libgcc-file-name)

# some defaults
EXE ?= sth_$(PLATFORM)
HARDWARE_ADDRESS ?= 0x8000C000
# HARDWARE_ADDRESS = 0x80000000
# build rules
all: build/$(EXE).img

build/$(EXE).img: build/$(EXE).elf
	$(MKIMAGE) -A arm -O linux -T kernel -C none -a $(HARDWARE_ADDRESS) -e $(HARDWARE_ADDRESS) -n build/$(EXE) -d build/$(EXE).bin build/$(EXE).img


build/$(EXE).elf: $(OBJ_FILES) build $(LSCRIPT) Makefile
	$(LD) -o $@ $(OBJ_FILES) -T $(LSCRIPT) $(LFLAGS) -Map build/$(EXE).map
        # some extra stuff for debugging
	$(OBJDUMP) -M reg-names-raw -D -x -t -r build/$(EXE).elf > build/$(EXE).asm
	$(STRIP) build/$(EXE).elf -o build/$(EXE).strip.elf
	$(OBJDUMP) -M reg-names-raw -D -x -t -r build/$(EXE).strip.elf > build/$(EXE).strip.asm
	$(OBJCOPY) build/$(EXE).elf -O binary build/$(EXE).bin
	od -t x4 build/$(EXE).bin > build/$(EXE).bin.hex


##

# simulation defined elsewhere

-include $(BASE)/core/config/sim/$(PLATFORM).inc

##

show: build/$(EXE).elf
	less  build/$(EXE).asm

clean:
	-rm build/$(EXE)*
	-rm $(OBJ_FILES)
	-make -C test clean

build:
	mkdir build

backup: clean
	tar cvfj sth.tar.bz2 *
	mv *bz2 "Y:\backup\code\misc"

#
.PHONY: test
test:
	@echo Compiling and running tests
	@env SRC_DIRS="$(SRC_DIRS)" INC_DIRS="$(INC_DIRS)" make -B -C test
