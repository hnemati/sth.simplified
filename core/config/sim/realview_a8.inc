# -*- mode: Makefile -*-

QEMU = qemu-system-arm -M realview-pb-a8 -m 128M
QEMU += -nographic -D qemu.log

GDB = arm-none-eabi-gdb -ex "target remote :1234"


sim: build/$(EXE).elf
	-killall qemu-system-arm

	$(QEMU) -kernel build/$(EXE).elf &

	@echo "************************************************"
	@echo "QEMU is now running."
	@echo "Run 'make dbg' to connect to it!"
	@echo "************************************************"


gdb: dbg

dbg:
	echo build/$(EXE).elf $(GUEST_IMAGE)
	$(GDB) -ex "symbol-file build/$(EXE).elf" \
	-ex "restore $(BASE)/guests/guest1/build/guest1.elf" \
	-ex "restore $(BASE)/guests/guest2/build/guest2.elf" \
	-ex "disp/i $$pc"

	-killall qemu-system-arm

#sim: build/$(EXE).elf
#	$(SIMULATOR) build/$(EXE).elf $(GUEST_IMAGE)
#	-cat *.log
#
#dbg: build/$(EXE).elf
#	$(SIMULATOR) build/$(EXE).elf $(GUEST_IMAGE) --debug
