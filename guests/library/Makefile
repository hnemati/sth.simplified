
TARGET=build/libtask
SRC_DIRS = src 
INC_DICS = ../../library/include


lib: build $(TARGET).a
	ls -l build

all: lib

include ../make.inc


$(TARGET).a: $(OBJ_FILES)
	$(AR) r $@ $^
	$(OBJDUMP) -M reg-names-raw -D -x -t -r $@ > $(TARGET).asm
