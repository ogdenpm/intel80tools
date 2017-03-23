ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REFDIR := ref

include $(ROOT)/tools/isistools.mk
# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

LINKOPT := 
PLMOPT :=  DEBUG
ASMOPT := 
# stack area allocated in the loc62.asm file
LOCATEOPT := SYMBOLS
OBJ = boot.obj t0boot.obj numout.obj consol.obj exit.obj write.obj

all: $(TARGETS)

isis.t0: $(OBJ) $(JUNKFILE)
	$(call link,isist0.rel,$^ $(call tools-v31,plm80.lib))
	$(call locate,isist0.abs,isist0.rel,name(isist0) stacksize(50) code(3000h))
	@del isist0.rel
ifdef JUNKFILE
	@echo ##################################################################
	@echo #### Memory overlaps are expected for address ranges:         ####
	@echo #### 3A96H-3AC7H, 3AC8H-3ACEH, 3ADCH-3ADDH, 3ADFH-3AE0H,      ####
	@echo #### 3AE3H-3AE8H, 3AF3H-3AF5H, 3AFFH-3B02H, 3B0AH-3B0DH,      ####
	@echo #### 3B12H-3B19H, 3B3DH-3B7FH                                 ####
	@echo ##################################################################
endif
	$(ROOT)/tools/obj2bin isist0.abs $@
	@del isist0.abs

