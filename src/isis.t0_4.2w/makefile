ITOOLS ?= ../..
TARGETS = isis.t0
PEXFILE := isist0.pex

include $(ITOOLS)/tools/isis.mk

REF=$(call ipath,isis,4.2w)

PATCH = isis.t0_4.2w.patch

LINKFLAGS := 
PLMFLAGS := DEBUG CODE
ASMFLAGS := DEBUG

PLM80 = 3.1

# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS PUBLICS

objs = boot.obj t0boot.obj config.obj aldadr.obj\
	   diskio.obj t0bota.obj consol.obj exit.obj memchk.obj write.obj

aldadr.obj: PLMFLAGS += NOOPTIMIZE

all::
	$(MAKE) $(TARGETS)

isis.t0: isist0.abs $(PATCH)
	$(call abstool,$@,$^)


isist0.abs: isist0.rel
	$(call locate,$@,$<,name(isist0) stacksize(36) code(3000h))

.INTERMEDIATE: isist0.rel
isist0.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

