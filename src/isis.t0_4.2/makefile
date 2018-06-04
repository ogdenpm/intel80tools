ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ROOT)/tools/isis.mk

REF=$(call ipath,isis,4.2)

# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

LINKFLAGS := 
PLMFLAGS := DEBUG CODE
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
	@echo "####    3AC4H-3AEDH, 3B0AH-3B0DH, 3B17H-3B1AH, 3B24H-3B27H,   ####"
	@echo "####    3B2DH-3B2DH, 3B51H-3B51H, and 3B62H-3B7F              ####"
	@echo "##################################################################"
endif

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs) $(JUNKFILE)
	$(call link,$@,$^ $(plm80.lib))

