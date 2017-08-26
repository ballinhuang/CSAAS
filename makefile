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
	$(MAKE) -C jobstate
clear:
	$(MAKE) -C lib clean
