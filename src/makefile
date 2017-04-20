
DIRS = asm80_4.1 disk43 isis34 isis34.t0 isis41 isis41.cli isis41.t0\
       isis43 isis43.cli isis43.ov0 isis43.t0 ixref_1.2 ixref_1.3 kermit\
       lib_2.1 link_3.0 locate_3.0 plm_4.0 plm80lib systemlib tex

# filter out the dirs that don't really support verify
VDIRS = $(filter-out plm80lib systemlib common,$(DIRS))

# dos needs 
mk = $(subst /,\,$(MAKE))

all:
	cmd /c "for %i in ($(DIRS)) do (pushd %i && ($(mk) all || exit 1) && popd)"

clean:
	cmd /c "for %i in ($(DIRS)) do (pushd %i && ($(mk) clean || exit 1) && popd)"

distclean:
	cmd /c "for %i in ($(DIRS)) do (pushd %i && $(mk) distclean && popd)"

rebuild:
	cmd /c "for %i in ($(DIRS)) do (pushd %i && $(mk) rebuild && popd)"

verify:
	cmd /c "for %i in ($(VDIRS)) do (pushd %i && $(mk) verify && popd)"

