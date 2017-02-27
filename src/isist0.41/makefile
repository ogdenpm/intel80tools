ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REFDIR := ref

include $(ROOT)/tools/isistools.mk
# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

LINKOPT := 
PLMOPT := 
ASMOPT := 
# stack area allocated in the loc62.asm file
LOCATEOPT := SYMBOLS
OBJ = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all: $(TARGETS)

isis.t0: $(OBJ) $(JUNKFILE)
	$(call link,isist0.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,isist0.abs,isist0.rel,name(isist0) stacksize(42) code(3000h))
	@del isist0.rel
ifdef JUNKFILE
	@echo ##################################################################
	@echo #### Memory overlaps are expected for address ranges:         ####
	@echo #### 3A81H-3AAAH, 3AC7H-3ACAH, 3AD4H-3AD7H, 3AE1H-3AE4H,      ####
	@echo #### 3AEAH-3AEAH, 3B0EH-3B0EH and 3B1FH-3B7FH                 ####
	@echo ##################################################################
endif
	$(ROOT)/tools/obj2bin isist0.abs $@
	@del isist0.abs
