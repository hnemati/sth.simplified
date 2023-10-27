
TARGET=minic
SRC_DIRS = src include


##

ASM_DIRS ?= SRC_DIRS
SRC_FILES = $(foreach dir,$(SRC_DIRS), $(wildcard $(dir)/*.c) )
ASM_FILES = $(foreach dir,$(ASM_DIRS), $(wildcard $(dir)/*.S) )
OBJ_FILES=$(SRC_FILES:.c=.o) $(ASM_FILES:.S=.o)
SRC_CFLAGS = $(foreach dir,$(SRC_DIRS), -I$(dir))
ASM_AFLAGS = $(foreach dir,$(ASM_DIRS), -I$(dir))



## TOOLCHAIN CONFIGURATION
ARMGNU = arm-none-eabi
CC=$(ARMGNU)-gcc
LD=$(ARMGNU)-ld
AS=$(ARMGNU)-as
AR=$(ARMGNU)-ar
STRIP=$(ARMGNU)-strip
OBJCOPY=$(ARMGNU)-objcopy
OBJDUMP=$(ARMGNU)-objdump

LIBGCC=$(shell $(CC) -print-libgcc-file-name)


CFLAGS += -Os -Wall  \
	-fomit-frame-pointer \
	-nostartfiles -nostdlib -ffreestanding \
	$(SRC_CFLAGS)

ASFLAGS = $(ASM_AFLAGS) 
##

all: build build/lib$(TARGET).a
	echo $(OBJ_FILES)
	ls -l build

build/lib$(TARGET).a: $(OBJ_FILES)
	$(AR) r $@ $^
	$(OBJDUMP) -M reg-names-raw -D -x -t -r $@ > build/lib$(TARGET).asm	


##
show: all
	less build/lib$(TARGET).asm
build:
	mkdir build
	
clean:
	rm -f $(OBJ_FILES)
	rm -rf build
