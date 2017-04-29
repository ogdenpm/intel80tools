ROOT=..
SHELL= $(ROOT)/unix/bash.exe

DIRS = asm80_4.1 disk43 isis34 isis34.t0 isis41 isis41.cli isis41.t0\
       isis43 isis43.cli isis43.ov0 isis43.t0 ixref_1.2 ixref_1.3 kermit\
       lib_2.1 link_3.0 locate_3.0 plm_4.0 plm80lib systemlib tex

all: 
	@for f in $(DIRS); do $(MAKE) -C $$f || exit 1; done;

clean: 
	@for f in $(DIRS); do $(MAKE) -C $$f clean || exit 1; done;

distclean: 
	@for f in $(DIRS); do $(MAKE) -C $$f distclean || exit 1; done;

rebuild: 
	@for f in $(DIRS); do $(MAKE) -C $$f rebuild || exit 1; done;

verify: 
	@for f in $(DIRS); do $(MAKE) -C $$f verify || exit 1; done;
