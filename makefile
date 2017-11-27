all:
	$(MAKE) libbuild;
	$(MAKE) componentbuild;
	$(MAKE) clear;
libbuild:
	$(MAKE) -C lib
componentbuild:
	$(MAKE) -C server
	$(MAKE) -C mom
	$(MAKE) -C scheduler
	$(MAKE) -C subjob
	$(MAKE) -C jobstatus
	$(MAKE) -C killjob
	$(MAKE) -C changemod
	$(MAKE) -C addnode
	$(MAKE) -C removenode
clear:
	$(MAKE) -C lib clean
