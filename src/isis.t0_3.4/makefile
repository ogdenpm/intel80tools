ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ROOT)/tools/isis.mk
# comment out below to omit junk filling - note verify will fail

REF=$(call ipath,isis,3.4)

JUNKFILE := junk.obj

LINKFLAGS := 
PLMFLAGS :=  DEBUG
ASMFLAGS := 
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS

objs = boot.obj t0boot.obj numout.obj consol.obj exit.obj write.obj

all:: $(TARGETS)

isis.t0: isist0.abs
	$(ROOT)/tools/obj2bin $^ $@

.INTERMEDIATE: isist0.rel

isist0.abs: isist0.rel
	$(call locate-overlaps,$@,$^,name(isist0) stacksize(50) code(3000h))
ifdef JUNKFILE
	@echo "##################################################################"
	@echo "#### Memory overlaps are expected for address ranges:         ####"
	@echo "#### 3A96H-3AC7H, 3AC8H-3ACEH, 3ADCH-3ADDH, 3ADFH-3AE0H,      ####"
	@echo "#### 3AE3H-3AE8H, 3AF3H-3AF5H, 3AFFH-3B02H, 3B0AH-3B0DH,      ####"
	@echo "#### 3B12H-3B19H, 3B3DH-3B7FH                                 ####"
	@echo "##################################################################"
endif

isist0.rel: $(objs) $(JUNKFILE)
	$(call link,$@,$^ $(plm80.lib))

