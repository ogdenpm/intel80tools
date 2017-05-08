ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REF := ref

include $(ROOT)/tools/isis.mk
# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

LINKFLAGS := 
PLMFLAGS := DEBUG
ASMFLAGS := DEBUG
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS PUBLICS

objs = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all:: $(TARGETS)

isis.t0: isist0.abs
	$(ROOT)/tools/obj2bin $< $@


isist0.abs: isist0.rel
	$(call locate-overlaps,$@,$<,name(isist0) stacksize(42) code(3000h))
ifdef JUNKFILE
	@echo "##################################################################"
	@echo "#### Memory overlaps are expected for address ranges:         ####"
	@echo "####    3AC9H-3AF2H, 3B0FH-3B12H, 3B1CH-3B1FH, 3B29H-3B2CH,   ####"
	@echo "####    3B32H-3B32H, 3B56H-3B56H, and 367H-3B7FH              ####"
	@echo "##################################################################"
endif

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs) $(JUNKFILE)
	$(call link,$@,$^ $(plm80.lib))
