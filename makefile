all:
	$(MAKE) -C server
	$(MAKE) -C qsub
	$(MAKE) -C mom
	$(MAKE) -C scheduler