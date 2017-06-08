ROOT=..
SHELL= $(ROOT)/unix/bash.exe

DIRS = asm80_4.1 isis.cli_4.1 isis.cli_4.3 isis.ov0_4.3 isis.t0_3.4 isis.t0_4.1\
       isis.t0_4.3 isisUtil_4.3 isis_3.4 isis_4.1 isis_4.3 ixref_1.2 ixref_1.3\
       kermit lib_2.1 link_3.0 locate_3.0 plm80.lib plm_4.0 system.lib_4.0 tex\
       toolbox_1.0 toolbox_2.0


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
