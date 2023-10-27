NOVLNV=1
IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")


#EXECUTABLE=platform.exe
#SRC=$(wildcard src/*.c)
#SHAREDOBJS=$(SRCS:.c=.o) $(ASM:.S=.o)
#WORKDIR=build
#
# include $(IMPERAS_HOME)/ImperasLib/source/buildutils/Makefile.platform
# all: $(WORKDIR) $(WORKDIR)/$(EXECUTABLE) $(WORKDIR)/$(SHAREDOBJ)
	


include $(IMPERAS_HOME)/bin/Makefile.include

ifndef IMPERAS_HOME
  IMPERAS_ERROR := $(error "IMPERAS_HOME not defined")
endif


SRC=$(wildcard src/*.c) $(wildcard ../ovp_common/*.c)
ASM=$(wildcard src/*.S)
OBJ=$(SRC:.c=.o) $(ASM:.S=.o)
EXE=platform

all: $(EXE).exe
# all: $(EXE).exe $(EXE).$(SHRSUF)

##

$(EXE).exe: $(OBJ)
	$(V) echo "Linking Simulation platform $@"
	$(V) gcc -g -o $@ $^ $(SIM_CFLAGS) $(SIM_LDFLAGS)

#$(EXE).$(SHRSUF): $(OBJ)
#	$(V) echo "Making Simulation platform"
#	$(V) gcc -shared -g -o $@ $^ $(SIM_CFLAGS) $(IMPERAS_ICMSTUBS)

##

%.o: %.c
	echo "Compiling Simulation platform $@"
	gcc -g -w -c -o $@ $< $(IMPERAS_VMIINC)


clean:
	-rm $(EXE).exe 
#	-rm $(EXE).$(SHRSUF)
	-rm $(OBJ)
