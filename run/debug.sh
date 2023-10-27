#!/bin/bash
# qemu-system-arm -M realview-pb-a8 -sdl -m 128M -kernel ../core/build/sth_realview_a8.img --nographic 
# qemu-system-arm -M beaglexm -m 512 -sd ~/build/priv/sth_deps/beagle_sd_working.img --nographic 
qemu-system-arm -M realview-pb-a8  -kernel ../core/build/sth_realview_a8.bin --nographic -S -s
