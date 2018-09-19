# path to root of build tree
ROOT := ../..
TARGETS := ftrans
PEXFILE:=ftrans.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,ftrans,1.0)
PROTECT := protocol.txt
# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:= OPTIMIZE DEBUG CODE
LINKFLAGS:=

objs = ftrans.obj cmdlin.obj txbyte.obj txdata.obj ioBuf.obj conect.obj\
	   multif.obj copied.obj ioerr.obj dtype.obj ferror.obj forupp.obj\
	   unpath.obj wpath.obj pseq.obj scanin.obj dmeq.obj dblank.obj meq.obj\
	   reperr.obj trange.obj upcase.obj wdlmt.obj

all:: $(TARGETS)

ftrans: ftrans.rel
	$(call locate,$@,$^,code(3680H) name(FTXMAI) stacksize(98h) purge)

.INTERMEDIATE: ftrans.rel
ftrans.rel: $(objs)
	$(call link,$@,$^ $(system.lib,4.0) $(plm80.lib))
