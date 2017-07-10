all:
	$(MAKE) -C lib
	$(MAKE) -C server
	$(MAKE) -C mom
	$(MAKE) -C scheduler
	$(MAKE) -C subjob
	$(MAKE) -C lib clean
