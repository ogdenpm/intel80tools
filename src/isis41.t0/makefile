ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REF := ref

include $(ROOT)/tools/isis.mk
# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

LINKFLAGS := 
PLMFLAGS := 
ASMFLAGS := 
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS
objs = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all:: $(TARGETS)

isis.t0: isist0.abs
	$(ROOT)/tools/obj2bin $^ $@

isist0.abs: isist0.rel
	$(call locate-overlaps,$@,$^,name(isist0) stacksize(42) code(3000h))
ifdef JUNKFILE
	@echo "##################################################################"
	@echo "#### Memory overlaps are expected for address ranges:         ####"
	@echo "####    3A81H-3AAAH, 3AC7H-3ACAH, 3AD4H-3AD7H, 3AE1H-3AE4H,   ####"
	@echo "####    3AEAH-3AEAH, 3B0EH-3B0EH and 3B1FH-3B7FH              ####"
	@echo "##################################################################"
endif

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs) $(JUNKFILE)
	$(call link,$@,$^ $(V40)/plm80.lib)

