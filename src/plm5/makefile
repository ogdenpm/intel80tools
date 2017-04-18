ROOT := ../..
SHARED := ../common
PEXFILE=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80.ov5
NAME = PL8050
STACK = 190

LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	main5.obj $(SHARED)/movmem.obj $(SHARED)/exit.obj $(SHARED)/itoa.obj\
	$(SHARED)/prints.obj $(SHARED)/initf.obj $(SHARED)/openf.obj $(SHARED)/closef.obj\
	$(SHARED)/creatf.obj $(SHARED)/fflush.obj $(SHARED)/fwrite.obj $(SHARED)/fread.obj\
	$(SHARED)/deletf.obj $(SHARED)/gitype.obj $(SHARED)/gisym.obj $(SHARED)/giscop.obj\
	$(SHARED)/siscop.obj $(SHARED)/gilnkv.obj $(SHARED)/tiflag.obj $(SHARED)/gieid.obj\
	$(SHARED)/gidim.obj $(SHARED)/gibseo.obj $(SHARED)/giparo.obj $(SHARED)/gidim2.obj\
	$(SHARED)/gibsev.obj $(SHARED)/gidtyp.obj $(SHARED)/giintn.obj $(SHARED)/adninf.obj\
	plm5a.obj $(SHARED)/lstinf.obj $(SHARED)/endcom.obj lstsp5.obj\
	$(SHARED)/fill.obj $(SHARED)/close.obj $(SHARED)/delete.obj $(SHARED)/open.obj\
	$(SHARED)/write.obj $(SHARED)/cpytil.obj $(SHARED)/fatlio.obj $(SHARED)/readf.obj\
	$(SHARED)/writef.obj $(SHARED)/gilen.obj $(SHARED)/tstflg.obj $(SHARED)/wr2lst.obj\
	page1.obj $(SHARED)/putlst.obj $(SHARED)/read.obj $(SHARED)/wrclst.obj\
	$(SHARED)/wrslst.obj page2.obj

all: $(TARGETS)

plm80.ov5: plm805.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm805.rel

plm805.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,publics($(SHARED)/data.abs) $(OBJ) $(V40)/plm80.lib)



# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))

