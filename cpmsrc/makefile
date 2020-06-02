ROOT=..
SHELL= $(ROOT)/unix/bash.exe

DIRS = lib_1.1 link_1.31 tex ml80


all: 
	@for f in $(DIRS) $(SPECIAL); do $(MAKE) -C $$f || exit 1; done;

clean: 
	@for f in $(DIRS); do $(MAKE) -C $$f clean || exit 1; done;

distclean: 
	@for f in $(DIRS) $(SPECIAL); do $(MAKE) -C $$f distclean || exit 1; done;

rebuild: 
	@for f in $(DIRS) $(SPECIAL); do $(MAKE) -C $$f rebuild || exit 1; done;

verify: 
	@for f in $(DIRS) $(SPECIAL); do $(MAKE) -C $$f verify || exit 1; done;
