ROOT := ../..
SHARED := shared
PEXFILE :=$(SHARED)/plm.pex
SRC := src
LST := lst
OBJ := obj


include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
export ISIS_F2 := $(SRC)/

PURGE := purge

TARGETS = plm80 plm80.ov0 plm80.ov1 plm80.ov2 plm80.ov3 plm80.ov4 plm80.ov5 plm80.ov6


LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

objs = main.obj plma.obj plmb.obj memchk.obj movmem.obj fill.obj plmc.obj\
       exit.obj prints.obj fatal.obj fatlio.obj initf.obj openf.obj readf.obj\
       chain.obj siflag.obj sidtyp.obj sipcnt.obj sibin.obj creati.obj\
       lookup.obj plmd.obj load.obj open.obj read.obj write.obj strcmp.obj\
       cpytil.obj itoa.obj silen.obj sitype.obj sisym.obj siscop.obj\
       silnko.obj setflg.obj allocs.obj alloc.obj plme.obj

objs0 = main0.obj plm0a.obj plm0b.obj plm0c.obj plm0d.obj plm0e.obj plm0f.obj\
	movmem.obj fill.obj strcmp.obj fatal.obj initf.obj openf.obj readf.obj\
       	tellf.obj seekf.obj rewind.obj backup.obj closef.obj creatf.obj\
	fflush.obj fwrite.obj chain.obj gitype.obj sitype.obj gisym.obj\
	giscop.obj gilit.obj silit.obj setflg.obj tstflg.obj zerflg.obj\
	clrflg.obj cpyflg.obj siflag.obj tiflag.obj sidim.obj sibseo.obj\
	giparo.obj siparo.obj gidtyp.obj sidtyp.obj siintn.obj gipcnt.obj\
	sipcnt.obj sipid.obj gicond.obj sicond.obj creati.obj findmi.obj\
	plm0g.obj adninf.obj fiscop.obj fi.obj lookup.obj alloc.obj plm0h.obj\
	close.obj exit.obj load.obj open.obj read.obj seek.obj cpytil.obj\
	prints.obj fatlio.obj writef.obj gilen.obj silen.obj sisym.obj\
	siscop.obj gilnko.obj silnko.obj allocs.obj write.obj itoa.obj

objs1 = main1.obj plm1a.obj plm1b.obj plm1c.obj plm1d.obj plm1e.obj plm1f.obj\
	movmem.obj openf.obj rewind.obj closef.obj creatf.obj fflush.obj\
	fwrite.obj fread.obj chain.obj deletf.obj gitype.obj sisym.obj\
	giscop.obj gilnko.obj gilnkv.obj silnkv.obj clrflg.obj siflag.obj\
	tiflag.obj gieid.obj sieid.obj gidim.obj sidim.obj gibseo.obj\
	giparo.obj siparo.obj gidtyp.obj gipcnt.obj gibin.obj creati.obj\
	findmi.obj adninf.obj fi.obj alloc.obj fill.obj close.obj delete.obj\
	load.obj open.obj fatlio.obj initf.obj readf.obj writef.obj seekf.obj\
	gilen.obj silen.obj sitype.obj siscop.obj silnko.obj setflg.obj\
	tstflg.obj fiscop.obj exit.obj read.obj seek.obj write.obj cpytil.obj\
	itoa.obj prints.obj

objs2 =	main2.obj plm2a.obj plm2b.obj plm2c.obj plm2d.obj plm2e.obj plm2f.obj\
	plm2g.obj plm2h.obj movmem.obj fill.obj fatal.obj creatf.obj\
	fflush.obj fwrite.obj fread.obj chain.obj gitype.obj gilnkv.obj\
	silnkv.obj tiflag.obj gieid.obj gidim.obj giparo.obj sidim.obj\
	gibsev.obj sibsev.obj giparv.obj gidtyp.obj gipcnt.obj gipid.obj\
	adninf.obj exit.obj load.obj prints.obj fatlio.obj initf.obj readf.obj\
	writef.obj gilen.obj tstflg.obj read.obj write.obj cpytil.obj itoa.obj

objs3 =	main3.obj plm3a.obj exit.obj itoa.obj openf.obj rewind.obj sekend.obj\
	closef.obj creatf.obj fflush.obj fwrite.obj fread.obj chain.obj\
	deletf.obj gitype.obj gisym.obj giscop.obj gilnkv.obj silnkv.obj\
	tiflag.obj gieid.obj gidim.obj giparo.obj gidim2.obj gibsev.obj\
	giintn.obj adninf.obj plm3b.obj endcom.obj movmem.obj fill.obj\
	close.obj delete.obj load.obj open.obj seek.obj prints.obj fatlio.obj\
	initf.obj readf.obj writef.obj seekf.obj gilen.obj tstflg.obj read.obj\
	write.obj cpytil.obj

objs4 =	pdata4.obj main4.obj plm4a.obj plm4b.obj plm4c.obj movmem.obj exit.obj\
	itoa.obj fatal.obj initf.obj openf.obj readf.obj tellf.obj seekf.obj\
	backup.obj closef.obj creatf.obj fflush.obj fwrite.obj fread.obj\
	chain.obj deletf.obj gitype.obj gisym.obj gilnkv.obj tiflag.obj\
	gieid.obj wrec.obj lstinf.obj endcom.obj putlst.obj lstsp4.obj\
	lstlin.obj fill.obj close.obj delete.obj load.obj open.obj read.obj\
	seek.obj cpytil.obj prints.obj fatlio.obj writef.obj tstflg.obj\
	gibsev.obj wrclst.obj wr2lst.obj page.obj write.obj wrslst.obj

objs5 =	main5.obj movmem.obj exit.obj itoa.obj prints.obj initf.obj openf.obj\
	closef.obj creatf.obj fflush.obj fwrite.obj fread.obj deletf.obj\
	gitype.obj gisym.obj giscop.obj siscop.obj gilnkv.obj tiflag.obj\
	gieid.obj gidim.obj gibseo.obj giparo.obj gidim2.obj gibsev.obj\
	gidtyp.obj giintn.obj adninf.obj plm5a.obj lstinf.obj endcom.obj\
	lstsp5.obj fill.obj close.obj delete.obj open.obj write.obj cpytil.obj\
	fatlio.obj readf.obj writef.obj gilen.obj tstflg.obj wr2lst.obj\
	page1.obj putlst.obj read.obj wrclst.obj wrslst.obj page2.obj

objs6 =	pdata6.obj main6.obj plm6a.obj plm6b.obj movmem.obj exit.obj itoa.obj\
	initf.obj openf.obj readf.obj tellf.obj seekf.obj backup.obj\
	closef.obj creatf.obj fflush.obj fread.obj chain.obj deletf.obj\
	gisym.obj lstinf.obj endcom.obj putlst.obj lstsp6.obj lstlin.obj\
	fill.obj close.obj delete.obj load.obj open.obj read.obj seek.obj\
	cpytil.obj prints.obj fatlio.obj writef.obj gibsev.obj wrclst.obj\
	wr2lst.obj page.obj write.obj wrslst.obj

# the following require plm v3.1
$(addprefix $(OBJ)/,plm1b.obj plm2b.obj plm2g.obj): ISISTOOLS=$(V31)

# add the extra place to look for source
vpath %.plm $(SHARED)
vpath %.asm $(SHARED)

all:: $(TARGETS)

plm80: $(OBJ)/plm80.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8010) stacksize(136) $(PURGE))

plm80.ov0: $(OBJ)/plm800.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8020) stacksize(314) $(PURGE))

plm80.ov1: $(OBJ)/plm801.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8025) stacksize(274) $(PURGE))

plm80.ov2: $(OBJ)/plm802.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8040) stacksize(240) $(PURGE))

plm80.ov3: $(OBJ)/plm803.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8042) stacksize(194) $(PURGE))

plm80.ov4: $(OBJ)/plm804.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8045) stacksize(266) $(PURGE))

plm80.ov5: $(OBJ)/plm805.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8050) stacksize(190) $(PURGE))

plm80.ov6: $(OBJ)/plm806.rel
	$(call locate,$@,$<,code(3EA0H) name(PL8043) stacksize(224) $(PURGE))

# the intermediate relocatable file build dependencies and rules
.INTERMEDIATE: $(addprefix $(OBJ)/,plm80.rel plm800.rel plm801.rel plm802.rel\
	                           plm803.rel plm804.rel plm805.rel plm806.rel) 

$(OBJ)/plm80.rel: $(call objdir,data.abs $(objs))
	$(call link,$@,$^ $(V40)/plm80.lib)

$(OBJ)/plm800.rel: $(call objdir,data.abs $(objs0))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs0)) $(V40)/plm80.lib)

$(OBJ)/plm801.rel: $(call objdir,data.abs $(objs1))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs1)) $(V40)/plm80.lib)

$(OBJ)/plm802.rel: $(call objdir,data.abs $(objs2))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs2)) $(V40)/plm80.lib)

$(OBJ)/plm803.rel: $(call objdir,data.abs $(objs3))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs3)) $(V40)/plm80.lib)

$(OBJ)/plm804.rel: $(call objdir,data.abs $(objs4))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs4)) $(V40)/plm80.lib)

$(OBJ)/plm805.rel: $(call objdir,data.abs $(objs5))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs5)) $(V40)/plm80.lib)

$(OBJ)/plm806.rel: $(call objdir,data.abs $(objs6))
	$(call link,$@,publics($(OBJ)/data.abs) $(call objdir,$(objs6)) $(V40)/plm80.lib)

$(OBJ)/data.abs: $(OBJ)/data.obj
	$(call locate,$@,$<,DATA(3800H))

# extra rule for clean
clean::
	-rm -f $(SHARED)/*.ipx

# source files needed to force make to see dependency on extracted files
SHAREDSRC = plm.pex adninf.plm alloc.plm allocs.plm backup.plm chain.plm\
	    ciflag.plm close.plm closef.plm clrflg.plm cpyflg.plm cpytil.plm\
	    creatf.plm creati.plm data.plm delete.plm deletf.plm endcom.plm\
	    error.plm exit.plm fatal.plm fatlio.plm fflush.plm fi.plm\
	    findmi.plm fiscop.plm gibin.plm gibseo.plm gibsev.plm gicond.plm\
	    gidim.plm gidim2.plm gidtyp.plm gieid.plm giintn.plm gilen.plm\
	    gilit.plm gilnko.plm gilnkv.plm giparo.plm giparv.plm gipcnt.plm\
	    gipid.plm giscop.plm gisym.plm gitype.plm initf.plm itoa.plm\
	    lstsup.cmn load.plm lookup.plm lstinf.plm lstlin.plm open.plm\
	    openf.plm prints.plm read.plm readf.plm rename.plm rescan.plm\
	    rewind.plm seek.plm seekf.plm sekend.plm setflg.plm sibin.plm\
	    sibseo.plm sibsev.plm sicond.plm sidim.plm sidtyp.plm sieid.plm\
	    siflag.plm siintn.plm silen.plm silit.plm silnko.plm silnkv.plm\
	    siparo.plm sipcnt.plm sipid.plm siscop.plm sisym.plm sitype.plm\
	    strcmp.plm tellf.plm tiflag.plm tstflg.plm wr2lst.plm write.plm\
	    writef.plm wrslst.plm zerflg.plm fill.asm fread.asm fwrite.asm\
	    memchk.asm movmem.asm putlst.asm wrclst.asm
MAINSRC =   lstsp4.plm lstsp5.plm lstsp6.plm main.plm main0.plm main1.plm\
	    main2.plm main3.plm main4.plm main5.plm main6.plm page.plm\
	    page1.plm page2.plm pdata4.plm pdata6.plm plm0a.plm plm0b.plm\
	    plm0d.plm plm0e.plm plm0f.plm plm0g.plm plm0h.plm plm1a.plm\
	    plm1b.plm plm1c.plm plm1d.plm plm1e.plm plm1f.plm plm2a.plm\
	    plm2b.plm plm2c.plm plm2d.plm plm2e.plm plm2f.plm plm2g.plm\
	    plm2h.plm plm3a.plm plm3b.plm plm4a.plm plm4b.plm plm4c.plm\
	    plm5a.plm plm6a.plm plm6b.plm plma.plm plmb.plm plmc.plm plmd.plm\
	    plme.plm wrec.plm plm0c.asm

$(addprefix $(SHARED)/,$(SHAREDSRC)) $(addprefix $(SRC)/,$(MAINSRC)): .extract
