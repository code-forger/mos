all: init-code banner_right-code banner_left-code time-code
	cp init/init ../isodir/boot
	cp banner_right/banner_right ../isodir/boot
	cp banner_left/banner_left ../isodir/boot
	cp time/time ../isodir/boot

.PHONY init-code:
	$(MAKE) -C init

.PHONY banner_right-code:
	$(MAKE) -C banner_right

.PHONY banner_left-code:
	$(MAKE) -C banner_left

.PHONY time-code:
	$(MAKE) -C time