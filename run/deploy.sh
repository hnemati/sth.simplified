#!/bin/bash
sudo mount -o loop,offset=$[106496*512] ~/build/priv/sth_deps/beagle_sd_working.img ../mnt/
sudo cp ../core/build/sth_realview_a8.img ../mnt/boot/uImage
#sudo cp ../sth_sics/sth_dynamic/core/build/sth_beagleboard.fw.img ../mnt/boot/uImage
sudo umount ../mnt/
