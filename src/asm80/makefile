# path to root of build tree
ROOT:=../..
# path to build directories
SRCDIR:=src
LSTDIR:=lst
OBJDIR:=obj

include $(ROOT)/tools/isistools.mk

# set default tools disk
TOOLDSK=$(V31DSK)


vpath %.plm $(SRCDIR) $(OBJDIR)
vpath %.plx $(SRCDIR)
vpath %.asm $(SRCDIR)

# these are special build rules to preprocess the plx files
$(OBJDIR)/%m.plm: %.plx
	$(PLMPP) -sMACRO -o $(call mapdisk,$@ $<)

$(OBJDIR)/%n.plm: %.plx
	$(PLMPP) -o $(call mapdisk,$@ $<)

$(OBJDIR)/%s.plm: %.plx
	$(PLMPP) -sSMALL -o $(call mapdisk,$@ $<)

$(OBJDIR)/%b.plm: %.plx
	$(PLMPP) -sBIG -o $(call mapdisk,$@ $<)


# the core build script follows
# build options
RELEASETARGETS := asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5 asxref

# base address of the overlays and keyn.abs
SMALLOVL:=6D4Ch
BIGCODE:=3780h
KEYCODE:=075A2h

LOCATEOPT:=SYMBOLS LINES
PURGE:=PURGE
PLMFLAGS:=DEBUG

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

.PHONY: all verify rebuild clean distclean
all: .extract $(RELEASETARGETS)

.extract: asm80_all.plm | $(OBJDIR) $(LSTDIR)
	perl ..\..\unpack.pl
	date /t >.extract

$(OBJDIR):
	mkdir $(OBJDIR)

$(LSTDIR):
	mkdir $(LSTDIR)

verify: all
	cmd /c for %i in ($(RELEASETARGETS)) do fc /b %i $(call towin,$(V40))\%i

rebuild: distclean verify


## build rules to make the program and overlays
$(OBJDIR)/asm80: $(call objdir,root.abs keyn.abs asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3)
	$(call link,asm80.rel,$(call objdisk,root.abs keyn.abs asm80.ov3) publics($(call objdisk,asm80.ov0 asm80.ov1 asm80.ov2)))
	$(call locate,$@,asm80.rel,NAME(ASM80) STACKSIZE(0))

$(OBJDIR)/asm80.ov0: $(call objdir,cprite.obj cntrln.obj root.abs)
	$(call link,asmov0.rel,$(call objdisk,cprite.obj cntrln.obj) publics($(call objdisk,root.abs)))
	$(call locate,$@,asmov0.rel,NAME(A80O0) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJDIR)/asm80.ov1: $(call objdir,cprite.obj listn.obj root.abs)
	$(call link,asmov1.rel,$(call objdisk,cprite.obj listn.obj) publics($(call objdisk,root.abs)) $(call tooldisk,plm80.lib))
	$(call locate,$@,asmov1.rel,NAME(A80O1) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJDIR)/asm80.ov2: $(call objdir,cprite.obj emitn.obj root.abs)
	$(call link,asmov2.rel,$(call objdisk,cprite.obj emitn.obj) publics($(call objdisk,root.abs)))
	$(call locate,$@,asmov2.rel,NAME(A80O2) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJDIR)/asm80.ov3: $(call objdir,cprite.obj inits.obj root.abs keyn.abs)
	$(call link,asmov3.rel,$(call objdisk,cprite.obj inits.obj) publics($(call objdisk,root.abs keyn.abs)))
	$(call locate,$@,asmov3.rel,NAME(A80O3) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJDIR)/asm80.ov4: $(call objdir,$(MACROOBJS) keym.obj)
	$(call link,asmov4.rel,$(call objdisk,$(MACROOBJS)) $(call tooldisk,system.lib plm80.lib) $(call objdisk,keym.obj))
	$(call locate,$@,asmov4.rel,NAME(A80O4) CODE($(BIGCODE)) STACKSIZE(144))

$(OBJDIR)/asm80.ov5: $(call objdir,$(BIGOBJS) keyn.obj)
	$(call link,asmov5.rel,$(call objdisk,$(BIGOBJS)) $(call tooldisk,system.lib plm80.lib) $(call objdisk,keyn.obj))
	$(call locate,$@,asmov5.rel,NAME(A80O5) CODE($(BIGCODE)) STACKSIZE(144))

# locate deletes the input file to clean up. copy means we keep the .obj file
$(OBJDIR)/asxref: $(call objdir,asxref.obj)
	@copy $(addprefix $(OBJDIR)\,asxref.obj asxref.rel)
	$(call locate,$@,asxref.rel,CODE(3680h) STACKSIZE(32))

#
## rules for intermediate files
#
$(OBJDIR)/root.abs: $(call objdir,$(ROOTOBJS))
	$(call link-nocheck,root.rel,$(call objdisk,$(ROOTOBJS)) $(call tooldisk,system.lib plm80.lib)) >nul
	$(call locate-nocheck,$@,root.rel,STACKSIZE(120)) >nul

#
$(OBJDIR)/keyn.abs: $(call objdir,keyn.obj edata.obj)
	$(call link-nocheck,keyn.rel,$(call objdisk,keyn.obj edata.obj))
	$(call locate-nocheck,$@,keyn.rel,CODE($(KEYCODE)) STACKSIZE(0)) 

## housekeeping rules
clean:
ifneq '$(OBJDIR)' '.'
	-rmdir /s /q $(OBJDIR) >nul
else
	-del /q *.obj *.abs >nul
endif
ifneq '$(LSTDIR)' '.'
	-rmdir /s /q $(LSTDIR) >nul
else
	-del /q *.lst *.lin *.map >nul
endif

distclean: clean
	-del /q $(RELEASETARGETS) >nul
ifneq '$(SRCDIR)' '.'
	-rmdir /s /q $(SRCDIR) >nul
endif
	-del /q $(call towin,$(filter-out makefile,$(filter-out asm80_all.plm,$(wildcard *.*)))) .extract >nul
	perl ../../unpack.pl


