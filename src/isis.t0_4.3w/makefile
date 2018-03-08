ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ROOT)/tools/isis.mk

REF=$(call ipath,isis,4.3w)

# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

LINKFLAGS := 
PLMFLAGS := DEBUG CODE
ASMFLAGS := DEBUG
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS PUBLICS

objs = boot.obj t0boot.obj config.obj aldadr.obj\
	   diskio.obj t0bota.obj consol.obj exit.obj memchk.obj write.obj

all:: $(TARGETS)

isis.t0: isist0.abs
	$(ROOT)/tools/obj2bin $< $@


isist0.abs: isist0.rel
	$(call locate-overlaps,$@,$<,name(isist0) stacksize(36) code(3000h))
ifdef JUNKFILE
	@echo "##################################################################"
	@echo "#### Memory overlaps are expected for address ranges:         ####"
	@echo "####    3A9BH-3ABEH, 3B08H-3B0BH, 3B15H-3B18H, 3B22H-3B25H,   ####"
	@echo "####    3B2BH-3B2BH, 3B4FH-3B4FH and 3B5CH-3B7FH              ####"
	@echo "##################################################################"
endif

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs) $(JUNKFILE)
	$(call link,$@,$^ $(plm80.lib))


