# -*- mode: make -*-
#
# shared configuration for all tasks and task libraries
#

#TARGET_ARCH= -mcpu=cortex-a8 -mfloat-abi=softfp
#TARGET_FAMILY=arm_cortex_a8



LDSCRIPT  ?= ../library/lnkr2.ld




## TOOLCHAIN CONFIGURATION
ARMGNU = arm-none-eabi
CC=$(ARMGNU)-gcc
LD=$(ARMGNU)-ld 
AS=$(ARMGNU)-as
AR=$(ARMGNU)-ar

#DBG=$(ARMGNU)-gdb
STRIP=$(ARMGNU)-strip
OBJCOPY=$(ARMGNU)-objcopy
OBJDUMP=$(ARMGNU)-objdump


LIBGCC=$(shell $(CC) -print-libgcc-file-name)


#
ASM_DIRS ?= $(SRC_DIRS)
SRC_FILES = $(foreach dir,$(SRC_DIRS), $(wildcard $(dir)/*.c) )
ASM_FILES = $(foreach dir,$(ASM_DIRS), $(wildcard $(dir)/*.S) )
OBJ_FILES=$(SRC_FILES:.c=.o) $(ASM_FILES:.S=.o)
SRC_CFLAGS = $(foreach dir,$(SRC_DIRS), -I$(dir)) $(foreach dir,$(INC_DIRS), -I$(dir))
ASM_AFLAGS = $(foreach dir,$(ASM_DIRS), -I$(dir)) $(foreach dir,$(INC_DIRS), -I$(dir))

# add 
#SRC_CFLAGS += -I ../library/include
#LFLAGS += -L ../library/build -ltask

LFLAGS += $(foreach dir,$(LIB_DIRS), -L$(dir))
LFLAGS += $(foreach file,$(LIB_FILES), -l$(file))


## 

CFLAGS += -Os -Wall  -Wl,--section-start=handlersc=0x500000 \
	-fomit-frame-pointer \
	-nostartfiles -nostdlib -ffreestanding \
	$(SRC_CFLAGS)

ASFLAGS = $(ASM_AFLAGS) -march=armv5 -c

#


##

all: build $(TARGET).hex $(TARGET).bin
	ls -l $(TARGET)*
    
$(TARGET).hex: $(TARGET).strip.elf
	$(OBJCOPY) -O ihex $(TARGET).strip.elf $(TARGET).hex

$(TARGET).bin: $(TARGET).strip.elf
	$(OBJCOPY) -O binary $(TARGET).strip.elf $(TARGET).bin

$(TARGET).elf: $(OBJ_FILES) Makefile ../make.inc
	$(LD) -T $(LDSCRIPT) -o $(TARGET).elf $(OBJ_FILES) $(LFLAGS) $(LIBGCC) -Map=$(TARGET).map
	$(OBJDUMP) -M reg-names-raw -D -x -t -r $(TARGET).elf > $(TARGET).asm

$(TARGET).strip.elf: $(TARGET).elf
	$(STRIP) $(TARGET).elf -o $(TARGET).strip.elf
	$(OBJDUMP) -M reg-names-raw -D -x -t -r $(TARGET).strip.elf > $(TARGET).strip.asm	


##

show: all
	less $(TARGET).asm

##

build:
	mkdir build
clean:
	rm -f $(OBJ_FILES)
	rm -rf build

