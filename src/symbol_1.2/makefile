# path to root of build tree
ITOOLS ?= ../..
TARGETS := symbol

# build options
LOCATEFLAGS:=MAP SYMBOLS PUBLICS
PLMFLAGS:=OPTIMIZE CODE DEBUG
LINKFLAGS:=
PLM80 = 3.1

include $(ITOOLS)/tools/isis.mk

# set default tools
REF:= ref


OBJS =	start.obj symbol.obj core.obj disply.obj suport.obj

all::
	$(MAKE) $(TARGETS)

symbol: symbol.rel
	$(call locate,$@,$^,code(3680H) stacksize(82) purge)

.INTERMEDIATE: symbol.rel
symbol.rel: $(OBJS)
	$(call link,$@,$^ $(plm80.lib) $(system.lib,3.1))

list: .extract symbol.rel
	$(call locate,tmp,symbol.rel,code(3680H) stacksize(82))
	perl $(ITOOLS)/tools/relst.pl tmp symbol.map $(OBJS:.obj=.lst)
	rm tmp 
