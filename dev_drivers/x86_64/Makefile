# How to use
# include the makefile for the device driver
# put it as a dependency for install drivers
# The Makefiles will do the rest

include dev_drivers/x86_64/ps_2/Makefile

dev_drivers := build/dev_drivers/ps_2.lib
dev_includes := dev_drivers/x86_64/ps_2/include

.PHONY: clean_drivers
clean_drivers:
	@rm -rf build/dev_drivers
	@echo "Cleaned Device Drivers"

