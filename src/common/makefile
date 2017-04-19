ROOT := ../..
PEXFILE := plm.pex

include $(ROOT)/tools/isis.mk
#
# force shared folder to be on isis drive :f3:
export ISIS_F3 := ./


OBJ=	adninf.obj alloc.obj allocs.obj backup.obj chain.obj close.obj \
	closef.obj clrflg.obj cpytil.obj creatf.obj creati.obj data.obj \
	delete.obj deletf.obj endcom.obj exit.obj fatal.obj \
	fatlio.obj fflush.obj fi.obj fill.obj findmi.obj fiscop.obj \
	fread.obj fwrite.obj gibin.obj gibseo.obj gibsev.obj gidim.obj \
	gidim2.obj gidtyp.obj gieid.obj giintn.obj gilen.obj gilnko.obj \
	gilnkv.obj giparo.obj giparv.obj gipcnt.obj gipid.obj giscop.obj \
	gisym.obj gitype.obj initf.obj itoa.obj load.obj lookup.obj \
	lstinf.obj lstlin.obj memchk.obj movmem.obj open.obj openf.obj \
	prints.obj putlst.obj read.obj readf.obj rewind.obj seek.obj \
	seekf.obj sekend.obj setflg.obj sibin.obj sibsev.obj sidim.obj \
	sidtyp.obj sieid.obj siflag.obj silen.obj silnko.obj silnkv.obj \
	siparo.obj sipcnt.obj siscop.obj sisym.obj sitype.obj strcmp.obj \
	tellf.obj tiflag.obj tstflg.obj wr2lst.obj wrclst.obj write.obj \
	writef.obj wrslst.obj \
	cpyflg.obj gicond.obj gilit.obj sibseo.obj sicond.obj siintn.obj \
	silit.obj sipid.obj

TARGETS:= data.abs 

all: $(TARGETS) $(OBJ)


data.abs: data.obj
	$(call locate,$@,$<,DATA(3800H))

