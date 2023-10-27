
#SUBDIRS = simulation library core drivers
# SUBDIRS = library simulation core  guests
SUBDIRS = library core  guests

all: config
	 set -e; for d in $(SUBDIRS); do $(MAKE) -C $$d ; done


clean: config
	 for d in $(SUBDIRS); do $(MAKE) clean -C $$d ; done


##
sim: all
	make -C core sim

dbg: all
	make -C core dbg


# create default config file
config:
	echo "# Target configuration" > config
	echo "#PLATFORM ?=ovp_arm9" >> config
	echo "#PLATFORM ?=u8500_ref" >> config
	echo "PLATFORM ?=realview_a8" >> config
	echo "SOFTWARE ?=guest*" >> config
