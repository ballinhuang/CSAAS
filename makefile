all:
	$(MAKE) -C lib
	$(MAKE) -C server
	$(MAKE) -C mom
	$(MAKE) -C scheduler
	$(MAKE) -C subjob
	$(MAKE) -C debugtool
	$(MAKE) -C lib clean
