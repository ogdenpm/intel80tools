ROOT:=../..
TARGETS = kermit

include $(ROOT)/tools/isistools.mk

OBJ =	md2ker.obj md2con.obj md2sen.obj md2rec.obj

all: $(TARGETS)

kermit: $(OBJ)
	$(call link,kermit.rel,$^ $(call tools-v40,system.lib plm80.lib))
	$(call locate,$@,kermit.rel,name(kermit) purge)
	@del kermit.rel
