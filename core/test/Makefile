

# add out own files

TEST_FILES=$(wildcard tests/*.c)
SRC_FILES = test.c
EXE_FILES=$(TEST_FILES:.c=.exe)

# INC_DIRS += ../hypervisor ../include ../lib ../guests/minimale .
# SRC_DIRS += ../hypervisor ../include ../lib ../guests/minimal

IN_DIRS += . tests
SRC_FILES +=  $(foreach dir,$(SRC_DIRS), $(wildcard ../$(dir)/*.c) )
MY_INC_DIRS = $(foreach dir,$(INC_DIRS), -I../$(dir) )

CFLAGS +=  -Os -std=c99 -w -DTEST -DDEBUG -fno-builtin-memcpy
CFLAGS += $(MY_INC_DIRS) -I .


##

all: parser.exe $(EXE_FILES) 
			
.PHONY: all

parser.exe: parser.c
	gcc parser.c -o parser.exe
	
%.exe: %.c parser.exe test.c
	@echo "*** TEST SET = `./parser.exe $< $(SRC_FILES)` ***"
	@gcc  $(CFLAGS) test.c `./parser.exe $< $(SRC_FILES)` -o $@
	@./$@

##

clean:
	rm -rf *.exe
	rm -rf tests/*.exe
	
