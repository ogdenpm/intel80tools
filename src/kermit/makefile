ITOOLS ?=../..
TARGETS = kermit
REF = ref

include $(ITOOLS)/tools/isis.mk

objs =	md2ker.obj md2con.obj md2sen.obj md2rec.obj

all::
	$(MAKE) $(TARGETS)

kermit: kermit.rel
	$(call locate,$@,$^,name(kermit) purge)

.INTERMEDIATE: kermit.rel
kermit.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

