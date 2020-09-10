ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ROOT)/tools/isis.mk

REF=$(call ipath,isis,3.4)

PATCH = isis.t0_3.4.patch
PROTECT = $(PATCH)

LINKFLAGS := 
PLMFLAGS :=  DEBUG
ASMFLAGS := 
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS

objs = boot.obj t0boot.obj numout.obj consol.obj exit.obj write.obj

all::
	$(MAKE) $(TARGETS)

isis.t0: isist0.abs $(PATCH)
	$(ROOT)/tools/obj2bin $^ $@

.INTERMEDIATE: isist0.rel

isist0.abs: isist0.rel
	$(call locate,$@,$^,name(isist0) stacksize(50) code(3000h))

isist0.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

