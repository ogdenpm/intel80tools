# path to root of build tree
ROOT := ../..
TARGETS := link link.ovl

# build options
LOCATEFLAGS:=SYMBOLS LINES PUBLICS
PLMFLAGS:=DEBUG
LINKFLAGS:=

include $(ROOT)/tools/isis.mk

# set default tools
COMPARE := $(ROOT)/tools/diffbin

PEXFILE:=link.pex

LINKOBJ := link0.abs link1.abs link2.abs
LINKPUB := linkov.abs
LINKOVOBJ := linkov.obj
LINKOVPUB := link0.abs
OBJ0 :=	link.obj link1a.obj wrerr.obj memmov.obj errrpt.obj binasc.obj \
	strequ.obj skpspc.obj pstafn.obj pstfn.obj isis.obj isisen.obj
PUB1 =	link0.abs
OBJ1 :=	link3.obj
PUB2 = 	link0.abs link1.abs
OBJ2 :=	link3a.obj mkfnam.obj lineuc.obj
OBJOVL := linkov.obj

all:: $(TARGETS)

# link build rule
link: $(LINKOBJ) $(LINKPUB)
	$(call link,link.rel,$(LINKOBJ) publics($(LINKPUB)))
	$(call locate,$@,link.rel,STACKSIZE(0) COLUMNS(3) NAME(LINK) $(PURGE))
	@rm link.rel

link.ovl: $(LINKOVOBJ) $(LINKOVPUB)
	$(call link,linkov.rel,$(LINKOVOBJ) publics($(LINKOVPUB)) $(call v40dir,system.lib plm80.lib))
	$(call locate,$@,linkov.rel,code(48DEH) stacksize(0) name(LINKOVL) $(PURGE))
	@rm linkov.rel

link0.abs: $(OBJ0)
	$(call link-nocheck,link0.rel,$^ $(call v40dir,plm80.lib))
	$(call locate-nocheck,$@,link0.rel,STACKSIZE(84) COLUMNS(3))
	@rm link0.rel

link1.abs: $(OBJ1) $(PUB1)
	$(call link-nocheck,link1.rel,$(OBJ1) publics($(PUB1)) $(call v40dir,plm80.lib))
	$(call locate-nocheck,$@,link1.rel,CODE(48DEH) STACKSIZE(0))
	@rm link1.rel

link2.abs: $(OBJ2) $(PUB2)
	$(call link-nocheck,link2.rel,$(OBJ2) publics($(PUB2)) $(call v40dir,system.lib(delete) system.lib))
	$(call locate-nocheck,$@,link2.rel,CODE(629DH) STACKSIZE(0))
	@rm link2.rel

linkov.abs: $(OBJOVL)
	$(call locate-nocheck,$@,$<,CODE(48DEH) STACKSIZE(0))


