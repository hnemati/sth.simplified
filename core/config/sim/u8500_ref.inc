# -*- mode: Makefile -*-

# simulator, must be built separately
SIMULATOR= ../simulation/generic_a8/platform.exe

sim: build/$(EXE).elf
	$(SIMULATOR) build/$(EXE).elf $(GUEST_IMAGE)
	-cat *.log

dbg: build/$(EXE).elf
	$(SIMULATOR) build/$(EXE).elf $(GUEST_IMAGE) --debug
