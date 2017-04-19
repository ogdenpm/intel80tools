
DIRS = asm80_4.1 common disk43 isis34 isis34.t0 isis41 isis41.cli isis41.t0\
       isis43 isis43.cli isis43.ov0 isis43.t0 ixref_1.2 ixref_1.3 kermit\
       lib_2.1 link_3.0 locate_3.0 plm plm0 plm1 plm2 plm3 plm4 plm5 plm6\
       plm80lib systemlib tex

# filter out the dirs that don't really support verify
VDIRS = $(filter-out plm80lib systemlib common,$(DIRS))

all:
	cmd /c "for %i in ($(DIRS)) do (cd %i & $(MAKE) & cd ..)"

clean:
	cmd /c "for %i in ($(DIRS)) do (cd %i & $(MAKE) clean & cd ..)"

distclean:
	cmd /c "for %i in ($(DIRS)) do (cd %i & $(MAKE) distclean & cd ..)"

rebuild:
	cmd /c "for %i in ($(DIRS)) do (cd %i & $(MAKE) rebuild & cd ..)"

verify:
	cmd /c "for %i in ($(VDIRS)) do (cd %i & $(MAKE) verify & cd ..)"

