ITOOLS ?=..
SHELL= $(ITOOLS)/unix/bash.exe

DIRS = ab22_cpmbios asm80_4.1 edasm_1.1 fpal_2.1 ftrans_1.0 help_1.1 isdm3.2\
	   isis.cli_3.4 isis.cli_4.0 isis.cli_4.1 isis.cli_4.2 isis.cli_4.3\
	   isis.ov0_4.2 isis.ov0_4.3\
	   isis.t0_3.4 isis.t0_4.0 isis.t0_4.1 isis.t0_4.2 isis.t0_4.3 isis.t0_4.3w\
       isis_1.1 isis_1.2 isis_2.2 isis_3.4 isis_4.0 isis_4.1 isis_4.2 isis_4.2w isis_4.3 isis_4.3w\
	   isisUtil_3.4 isisUtil_4.3 isisUtil_4.3w ixref_1.2 ixref_1.3\
       kermit lib_2.1 link_3.0 locate_3.0 plm_4.0 plm80.lib system.lib_4.0\
       sdk51_1.03 toolbox_1.0 toolbox_2.0 utils_2.2n

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
