ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ROOT)/tools/isis.mk
# comment out below to omit junk filling - note verify will fail

REF=$(call ipath,isis,4.0)

JUNKFILE := junk.obj

LINKFLAGS := 
PLMFLAGS += DEBUG
ASMFLAGS := DEBUG
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS PUBLICS
objs = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all::
	$(MAKE) $(TARGETS)

isis.t0: isist0.abs
	$(ROOT)/tools/obj2bin $@ $<

isist0.abs: isist0.rel
	$(call locate-overlaps,$@,$^,name(isist0) stacksize(42) code(3000h))
ifdef JUNKFILE
	@echo "##################################################################"
	@echo "#### Memory overlaps are expected for address ranges:         ####"
	@echo "####    3A7CH-3AA5H, 3AC2H-3AC5H, 3ACFH-3AD2H, 3ADCH-3ADFH,   ####"
	@echo "####    3AE5H-3AE5H, 3B09H-3B09H, 3B1AH-3B7FH                 ####"
	@echo "##################################################################"
endif

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs) $(JUNKFILE)
	$(call link,$@,$^ $(plm80.lib))

