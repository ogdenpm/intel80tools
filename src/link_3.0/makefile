# path to root of build tree
ITOOLS ?= ../..
TARGETS := link link.ovl
PEXFILE:=link.pex

# build options
LOCATEFLAGS:=SYMBOLS LINES PUBLICS 
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=
PURGE:= purge

include $(ITOOLS)/tools/isis.mk

REF=$(call ipath,link,3.0)

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

all::
	$(MAKE) $(TARGETS)

# link build rules
.INTERMEDIATE: link.rel linkov.rel link0.rel link1.rel link2.rel

link: link.rel link.patch
	$(call locate,$@,link.rel,STACKSIZE(0) COLUMNS(3) NAME(LINK) $(PURGE))
	$(ITOOLS)/tools/fixobj -p link.patch $@ $@

link.rel: $(LINKOBJ) $(LINKPUB)
	$(call link,$@,$(LINKOBJ) $(call publics,$(LINKPUB)))

link.ovl: linkov.rel linkovl.patch
	$(call locate,$@,linkov.rel,code(48DEH) stacksize(0) name(LINKOVL) $(PURGE))
	$(ITOOLS)/tools/fixobj -p linkovl.patch $@ $@

linkov.rel: $(LINKOVOBJ) $(LINKOVPUB)
	$(call link,$@,$(LINKOVOBJ) $(call publics,$(LINKOVPUB)) $(system.lib) $(plm80.lib))

link0.abs: link0.rel
	$(call locate-externok,$@,$^,STACKSIZE(84) COLUMNS(3))

link0.rel: $(OBJ0)
	$(call link-externok,$@,$^ $(plm80.lib))

link1.abs: link1.rel
	$(call locate-externok,$@,$^,CODE(48DEH) STACKSIZE(0))

link1.rel: $(OBJ1) $(PUB1)
	$(call link-externok,$@,$(OBJ1) $(call publics,$(PUB1)) $(plm80.lib))

link2.abs: link2.rel
	$(call locate-externok,$@,$^,CODE(629DH) STACKSIZE(0))

link2.rel: $(OBJ2) $(PUB2)
	$(call link-externok,$@,$(OBJ2) $(call publics,$(PUB2)) "$(system.lib)(delete)" $(system.lib))

linkov.abs: $(OBJOVL)
	$(call locate-externok,$@,$<,CODE(48DEH) STACKSIZE(0))


