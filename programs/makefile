all: init-code banner_right-code banner_left-code time-code joeprogram-code joeprogram2-code joeprogram3-code pipe_test-code
	cp init/init ../isodir/boot
	cp banner_right/banner_right ../isodir/boot
	cp banner_left/banner_left ../isodir/boot
	cp time/time ../isodir/boot
	cp joeprogram/joeprogram ../isodir/boot
	cp joeprogram2/joeprogram2 ../isodir/boot
	cp joeprogram3/joeprogram3 ../isodir/boot
	cp pipe_test/pipe_test ../isodir/boot

.PHONY init-code:
	$(MAKE) -C init

.PHONY banner_right-code:
	$(MAKE) -C banner_right

.PHONY banner_left-code:
	$(MAKE) -C banner_left

.PHONY time-code:
	$(MAKE) -C time

.PHONY joeprogram-code:
	$(MAKE) -C joeprogram

.PHONY joeprogram2-code:
	$(MAKE) -C joeprogram2

.PHONY joeprogram3-code:
	$(MAKE) -C joeprogram3

.PHONY pipe_test-code:
	$(MAKE) -C pipe_test