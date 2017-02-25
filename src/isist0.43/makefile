ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REFDIR := ref

include $(ROOT)/tools/isistools.mk
# comment out below to omit junk filling - note verify will fail
FILLJUNK := T

LINKOPT := 
PLMOPT := 
ASMOPT := 
# stack area allocated in the loc62.asm file
LOCATEOPT := SYMBOLS
OBJ = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all: $(TARGETS)

ifdef FILLJUNK
isis.t0: $(OBJ) junk.obj
	$(call link,isist0.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,isist0.abs,isist0.rel,name(isist0) stacksize(42) code(3000h))
	@del isist0.rel
	@echo ##################################################################
	@echo #### Memory overlaps are expected for address ranges:         ####
	@echo #### 3AC9H-3AF2H, 3B0FH-3B12H, 3B1CH-3B1FH, 3B29H-3B2CH,      ####
	@echo #### 3B32H-3B32H, 3B56H-3B56H, and 367H-3B7FH                 ####
	@echo ##################################################################
	$(ROOT)/tools/obj2bin isist0.abs $@
	@del isist0.abs
else
isis.t0: $(OBJ)
	$(call link,isist0.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,isist0.abs,isist0.rel,name(isist0) stacksize(42) code(3000h))
	@del isist0.rel
	$(ROOT)/tools/obj2bin isist0.abs $@
	@del isist0.abs
endif

