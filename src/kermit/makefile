ROOT:=../..
TARGETS = kermit
REF = ref

include $(ROOT)/tools/isis.mk

OBJ =	md2ker.obj md2con.obj md2sen.obj md2rec.obj

all: $(TARGETS)

kermit: $(OBJ)
	$(call link,kermit.rel,$^ $(call v40dir,system.lib plm80.lib))
	$(call locate,$@,kermit.rel,name(kermit) purge)
	@rm kermit.rel
