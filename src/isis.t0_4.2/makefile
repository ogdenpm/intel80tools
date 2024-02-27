ITOOLS ?= ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ITOOLS)/tools/isis.mk

REF=$(call ipath,isis,4.2)

PATCH = isis.t0_4.2.patch

LINKFLAGS := 
PLMFLAGS := DEBUG CODE
ASMFLAGS := DEBUG
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS PUBLICS

objs = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all::
	$(MAKE) $(TARGETS)

isis.t0: isist0.abs $(PATCH)
	$(ABSBIN) $^ $@


isist0.abs: isist0.rel
	$(call locate,$@,$<,name(isist0) stacksize(42) code(3000h))

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

