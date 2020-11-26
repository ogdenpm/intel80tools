# path to root of build tree
ITOOLS ?= ..\..
TARGETS = run2-3.com basic.com
REF=ref

# setting BYTEMATCH to Y forces the build to match the distributed binary
# without it the compiler will generate standard code and no patches are needed
BYTEMATCH=Y



include $(ITOOLS)/tools/isis.mk


# core floating point library
FPOBJ = fpint.obj fpdata.obj fppkg.obj tran.obj fpconv.obj
# run linkable part
RUNOBJ = run.obj
# build linkable part
BUILDOBJ = build.obj
# marker object to reflect base of build module
BASEOBJ = bbase.obj
# basic linkable parts
BASICOBJ =  basic.obj bassyn.obj baspar.obj
# external link to cpm for gotos in MON1 & MON2
CPMOBJ = cpm.obj

ifeq '$(BYTEMATCH)' 'Y'
	EXACT=set(EXACT)
	RUNPATCH = run2-3.patch
	BASICPATCH = basic2-1.patch
	RUNFILES = $(RUNOBJ:.obj=.lob)
	BUILDFILES = $(BUILDOBJ:.obj=.lob)
	BASICFILES = $(BASICOBJ:.obj=.lob)
else
	RUNFILES = $(RUNOBJ)
	BUILDFILES = $(BUILDOBJ)
	BASICFILES = $(BASICOBJ)
endif

PROTECT = $(RUNPATCH) $(BASICPATCH) GettingToBinary.md GettingToBinary.pdf

PLMFLAGS := OPTIMIZE CODE DEBUG $(EXACT)
PLM80 := 4.0
ASMFLAGS := DEBUG
LOCATEFLAGS:= PUBLICS SYMBOLS


# basic uses F1 for bascom.lit
export ISIS_F1 :=./

.PHONY: all
all::
	$(MAKE) $(TARGETS)

# for basic each of the modules have their own customised extracts from the plm80.lib
# for run each of the two parts again have their own customised extracts from plm80.lib
# to prevent problems linking the modules which would reflect duplicate library functions
# intermediate .lob files are created from the main module and its bespoke library
# after which the @Pnnnn publics are removed
# as a convention for all the b*.plm files the bespoke libraries source are in x*.asm
# for run.plm, the bespoke library is in xrun.asm
# this convention allows rules to be used and avoids conflicts with .lst files

b%.lob: b%.obj x%.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

# run requires special handling as it doesn't start with a b
run.lob: run.obj xun.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

# create basic.com, if BYTEMATCH is set then it will use lob files
# else use standard compile and shared libraries
basic.rel: $(BASICFILES) $(CPMOBJ)
	$(call link,$@,$^ $(plm80.lib))

basic.abs: basic.rel
	$(call locate,$@,$^,code(100h))

basic.com: basic.abs $(BASICPATCH)
	$(OBJBIN) -j $^ $@

# the run.com build has two main modules so the fp routines and run itself
# need to be built and have the @Pnnnn references and the main module flag removed
fprun.rel: $(FPOBJ) $(RUNFILES)
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -n $@ $@

fprun.abs: fprun.rel
	$(call locate-nocheck,$@,$^,CODE(100h))

run23.rel: fprun.abs $(BASEOBJ) $(BUILDFILES)
	$(call link,$@,$^ $(plm80.lib))

run23.abs: run23.rel
	$(call locate,$@,$^,code(2b00h))

run2-3.com: run23.abs $(RUNPATCH)
	$(OBJBIN) $^ $@


print: all
	perl $(ITOOLS)/tools/relst.pl run.abs run.map $(FPOBJ:.obj=.lst) $(RUNOBJ:.obj=.lst) $(BUILDOBJ:.obj=.lst)

#  rule for src -> obj
$(OBJ)/%.obj: %.src  | $(OBJ) $(LST)
	$(call asm80,$@,$<)


clean::
	rm -fr *.rel *.prn *.abs *.lob
