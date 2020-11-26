ITOOLS ?=../..
TARGETS=system.lib
PLMFLAGS=code optimize 
ASMFLAGS=

include $(ITOOLS)/tools/isis.mk

REF=$(call ipath,utils,2.2n)

PLM80 := 3.1

memck.obj : ASM80 = 2.0

OBJS= 	attrib.ob1 ci.obj close.ob1 co.obj consol.ob1 csts.obj delete.ob1 exit.ob1\
	iochk.obj iodef.obj ioset.obj isis.obj lo.obj load.ob1 memck.obj open.ob1\
	po.obj read.ob1 rename.ob1 rescan.ob1 ri.obj seek.ob1 whocon.ob1 write.ob1\
	ui.obj uo.obj upps.obj spath.obj error.obj getatt.obj getd.obj\
	filinf.obj chgacs.obj detime.obj v1p5.obj

# pretend to be PLM v1.0
%.ob1: %.plm
	$(call plm80,$@,$<)
	$(ITOOLS)/tools/fixobj -v 10 $@ $@

all::
	$(MAKE) $(TARGETS)

system.lib: $(OBJS)
	$(call lib,$@,$^)

