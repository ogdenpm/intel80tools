# path to root of build tree
ITOOLS ?=../..
# path to build directories
SRC:=src
LST:=lst
OBJ:=obj

PEXFILE:=$(SRC)/asm80.pex

PROTECT := notes.txt

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,asm80,4.1)

# override default tools
PLM80 := 3.1
export ISIS_F3 = $(SRC)

TARGETS := asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5 asxref

# build options
LOCATEFLAGS:=SYMBOLS 
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=
PURGE:= purge


# this forces make to reinspect the *.plx files
all:: 
	$(MAKE) $(TARGETS)

# base address of the overlays and keyn.abs
SMALLOVL:=6D4Ch
BIGCODE:=3780h
KEYCODE:=075A2h


# object lists
ASMOBJ := root.abs keyn.abs asm80.ov3
ASMPUB := asm80.ov0 asm80.ov1 asm80.ov2 
OV0OBJ := cprite.obj cntrln.obj
OV0PUB := root.abs
OV1OBJ := cprite.obj listn.obj
OV1PUB := root.abs
OV2OBJ := cprite.obj emitn.obj
OV2PUB := root.abs
OV3OBJ := cprite.obj inits.obj
OV3PUB := root.abs keyn.abs

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

KEYOBJS := keyn.obj edata.obj

## build rules to make the program and overlays
# Symbols for overlays 0,1,2 and 3 are used in the build of asm80 so the apps are created by purging the symbols
%: $(OBJ)/% ; $(call rm-symbols,$@,$<)

.INTERMEDIATE:: $(call objdir,asm80.rel asm800.rel asm801.rel asm802.rel asm803.rel asm804.rel asm805.rel)
asm80: $(OBJ)/asm80.rel
	$(call locate,$@,$^,NAME(ASM80) STACKSIZE(0) $(PURGE))

$(OBJ)/asm80.rel: $(call objdir,$(ASMOBJ) $(ASMPUB))
	$(call link,$@,$(call objdir,$(ASMOBJ)) "publics($(call objdir,$(ASMPUB)))")

##
$(OBJ)/asm80.ov0: $(OBJ)/asm800.rel
	$(call locate,$@,$^,NAME(A80O0) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJ)/asm800.rel: $(call objdir,$(OV0OBJ) $(OV0PUB))
	$(call link,$@,$(call objdir,$(OV0OBJ)) "publics($(call objdir,$(OV0PUB)))")

##
$(OBJ)/asm80.ov1: $(OBJ)/asm801.rel
	$(call locate,$@,$^,NAME(A80O1) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJ)/asm801.rel: $(call objdir,$(OV1OBJ) $(OV1PUB))
	$(call link,$@,$(call objdir,$(OV1OBJ)) "publics($(call objdir,$(OV1PUB)))" $(plm80.lib))

##	
$(OBJ)/asm80.ov2: $(OBJ)/asm802.rel
	$(call locate,$@,$^,NAME(A80O2) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJ)/asm802.rel: $(call objdir,$(OV2OBJ) $(OV2PUB))
	$(call link,$@,$(call objdir,$(OV2OBJ)) "publics($(call objdir,$(OV2PUB)))")

##
$(OBJ)/asm80.ov3: $(OBJ)/asm803.rel
	$(call locate,$@,$^,NAME(A80O3) STACKSIZE(0) CODE($(SMALLOVL)))

$(OBJ)/asm803.rel: $(call objdir,$(OV3OBJ) $(OV3PUB))
	$(call link,$@,$(call objdir,$(OV3OBJ)) "publics($(call objdir,$(OV3PUB)))")

##
asm80.ov4: $(OBJ)/asm804.rel
	$(call locate,$@,$^,NAME(A80O4) CODE($(BIGCODE)) STACKSIZE(144) $(PURGE))

$(OBJ)/asm804.rel: $(call objdir,$(MACROOBJS) keym.obj)
	$(call link,$@,$(call objdir,$(MACROOBJS)) $(system.lib,3.1) $(plm80.lib) $(call objdir,keym.obj))

##
asm80.ov5: $(OBJ)/asm805.rel
	$(call locate,$@,$^,NAME(A80O5) CODE($(BIGCODE)) STACKSIZE(144) $(PURGE))

$(OBJ)/asm805.rel: $(call objdir,$(BIGOBJS) keyn.obj)
	$(call link,$@,$(call objdir,$(BIGOBJS)) $(system.lib,3.1) $(plm80.lib) $(call objdir,keyn.obj))

##
asxref: $(OBJ)/asxref.obj
	$(call locate,$@,$<,CODE(3680h) STACKSIZE(32) $(PURGE))

#
## rules for intermediate files
.INTERMEDIATE:: $(call objdir,root.rel keyn.rel)

$(OBJ)/root.abs: $(OBJ)/root.rel
	$(call locate-nocheck,$@,$^,STACKSIZE(120)) 

$(OBJ)/root.rel: $(call objdir,$(ROOTOBJS))
	$(call link-nocheck,$@,$^ $(system.lib,3.1) $(plm80.lib))

$(OBJ)/keyn.abs: $(OBJ)/keyn.rel
	$(call locate-nocheck,$@,$^,CODE($(KEYCODE)) STACKSIZE(0))

$(OBJ)/keyn.rel: $(call objdir,$(KEYOBJS))
	$(call link-nocheck,$@,$^)


# these are special build rules to process the plx files
$(OBJ)/%m.obj: $(SRC)/%.plx
	$(PLMPP) -sMACRO -o $(SRC)/$*m.plm $<
	$(call plm80,$@,$(SRC)/$*m.plm)

$(OBJ)/%n.obj: $(SRC)/%.plx
	$(PLMPP) -o $(SRC)/$*n.plm $<
	$(call plm80,$@,$(SRC)/$*n.plm)

$(OBJ)/%s.obj: $(SRC)/%.plx
	$(PLMPP) -sSMALL -o $(SRC)/$*s.plm $<
	$(call plm80,$@,$(SRC)/$*s.plm)

$(OBJ)/%b.obj: $(SRC)/%.plx
	$(PLMPP) -sBIG -o $(SRC)/$*b.plm $<
	$(call plm80,$@,$(SRC)/$*b.plm)



