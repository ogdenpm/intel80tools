# path to root of build tree
ROOT:=../..
# path to build directories
SRCDIR:=src
LSTDIR:=lst
OBJDIR:=obj

include $(ROOT)/tools/isistools.mk

# override default tools
ISISTOOLS := $(V31)

TARGETS := asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5 asxref

# build options
LOCATEOPT:=SYMBOLS LINES
PLMFOPT:=DEBUG
LINKOPT:=



all: $(TARGETS)

# base address of the overlays and keyn.abs
SMALLOVL:=6D4Ch
BIGCODE:=3780h
KEYCODE:=075A2h

PEXFILE:=$(SRCDIR)/asm80.pex

# bulk obj names
ROOTOBJS := globls.obj starts.obj asm1n.obj asm2n.obj\
	rdsrc.obj asm3s.obj asm4s.obj asm5n.obj\
	asm6n.obj pcktok.obj

MACROOBJS := globlm.obj startm.obj pcktok.obj asm1m.obj\
	asm2m.obj asm4m.obj rdsrc.obj asm3m.obj\
	asm5m.obj asm6m.obj cntrlm.obj asm8m.obj\
	emitm.obj listm.obj initm.obj

BIGOBJS := globlb.obj startb.obj pcktok.obj asm1n.obj\
	asm2n.obj asm4b.obj rdsrc.obj asm3b.obj\
	asm5n.obj asm6n.obj cntrln.obj emitn.obj\
	listn.obj initb.obj

## build rules to make the program and overlays rel files are intermediate so let make delete them
.INTERMEDIATE: $(call objdir,asm80.rel asm800.rel asm801.rel asm802.rel asm803.rel asm804.rel asm805.rel root.rel keyn.rel)

# Symbols for overlays 0,1,2 and 3 are used in the build of asm80 so the apps are created by purging the symbols

$(call objdir,asm80.rel: root.abs keyn.abs asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3)
	$(call link,$(call objdir,asm80.rel),$(call objdir,root.abs keyn.abs asm80.ov3) publics($(call objdir,asm80.ov0 asm80.ov1 asm80.ov2)))
asm80: $(call objdir,asm80.rel)
	$(call locate,$@,$<,NAME(ASM80) STACKSIZE(0) $(PURGE))

$(call objdir,asm800.rel: cprite.obj cntrln.obj root.abs)
	$(call link,$@,$(call objdir,cprite.obj cntrln.obj) publics($(call objdir,root.abs)))
$(call objdir,asm80.ov0: asm800.rel)
	$(call locate,$@,$<,NAME(A80O0) STACKSIZE(0) CODE($(SMALLOVL)))
asm80.ov0: $(OBJDIR)/asm80.ov0 ; $(call rm-symbols,$@,$<)

$(call objdir,asm801.rel: cprite.obj listn.obj root.abs)
	$(call link,$@,$(call objdir,cprite.obj listn.obj) publics($(call objdir,root.abs)) $(call tools-v31,plm80.lib))
$(call objdir,asm80.ov1: asm801.rel)
	$(call locate,$@,$<,NAME(A80O1) STACKSIZE(0) CODE($(SMALLOVL)))
asm80.ov1: $(OBJDIR)/asm80.ov1 ; $(call rm-symbols,$@,$<)
	
$(call objdir,asm802.rel: cprite.obj emitn.obj root.abs)
	$(call link,$@,$(call objdir,cprite.obj emitn.obj) publics($(call objdir,root.abs)))
$(call objdir,asm80.ov2: asm802.rel)
	$(call locate,$@,$<,NAME(A80O2) STACKSIZE(0) CODE($(SMALLOVL)))
asm80.ov2: $(OBJDIR)/asm80.ov2 ; $(call rm-symbols,$@,$<)

$(call objdir,asm803.rel: cprite.obj inits.obj root.abs keyn.abs)
	$(call link,$@,$(call objdir,cprite.obj inits.obj) publics($(call objdir,root.abs keyn.abs)))
$(call objdir,asm80.ov3: asm803.rel)
	$(call locate,$@,$<,NAME(A80O3) STACKSIZE(0) CODE($(SMALLOVL)))
asm80.ov3: $(OBJDIR)/asm80.ov3 ; $(call rm-symbols,$@,$<)

$(call objdir,asm804.rel: $(MACROOBJS) keym.obj)
	$(call link,$@,$(call objdir,$(MACROOBJS)) $(call tools-v31,system.lib plm80.lib) $(call objdir,keym.obj))
asm80.ov4: $(call objdir,asm804.rel)
	$(call locate,$@,$<,NAME(A80O4) CODE($(BIGCODE)) STACKSIZE(144) $(PURGE))

$(call objdir,asm805.rel: $(BIGOBJS) keyn.obj)
	$(call link,$@,$(call objdir,$(BIGOBJS)) $(call tools-v31,system.lib plm80.lib) $(call objdir,keyn.obj))
asm80.ov5: $(call objdir,asm805.rel)
	$(call locate,$@,$<,NAME(A80O5) CODE($(BIGCODE)) STACKSIZE(144) $(PURGE))

asxref: $(call objdir,asxref.obj)
	$(call locate,$@,$<,CODE(3680h) STACKSIZE(32) $(PURGE))

#
## rules for intermediate files
#
$(call objdir,root.rel: $(ROOTOBJS))
	$(call link-nocheck,$@,$^ $(call tools-v31,system.lib plm80.lib))
$(call objdir,root.abs: root.rel)
	$(call locate-nocheck,$@,$<,STACKSIZE(120)) 

#
$(call objdir,keyn.rel: keyn.obj edata.obj)
	$(call link-nocheck,$@,$^)
$(call objdir,keyn.abs: keyn.rel)
	$(call locate-nocheck,$@,$<,CODE($(KEYCODE)) STACKSIZE(0)) 


# these are special build rules to preprocess the plx files
$(OBJDIR)/%m.plm: %.plx
	$(PLMPP) -sMACRO -o $(call mapdisk,$@ $<)

$(OBJDIR)/%n.plm: %.plx
	$(PLMPP) -o $(call mapdisk,$@ $<)

$(OBJDIR)/%s.plm: %.plx
	$(PLMPP) -sSMALL -o $(call mapdisk,$@ $<)

$(OBJDIR)/%b.plm: %.plx
	$(PLMPP) -sBIG -o $(call mapdisk,$@ $<)
