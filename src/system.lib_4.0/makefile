ROOT=../..
TARGETS=system.lib
PLMFFLAGS=code optimize
ASMFFLAGS=

include $(ROOT)/tools/isis.mk

REF=$(call ipath,system.lib,4.0)

PLM80 := 3.1

OBJS= 	attrib.obj ci.obj close.obj co.obj consol.obj csts.obj delete.obj exit.obj\
	iochk.obj iodef.obj ioset.obj isis.obj lo.obj load.obj memck.obj open.obj\
	po.obj read.obj rename.obj rescan.obj ri.obj seek.obj whocon.obj write.obj\
	ui.obj uo.obj upps.obj spath.obj error.obj getatt.obj getd.obj\
	filinf.obj chgacs.obj detime.obj v1p5.obj

all:: $(TARGETS)

system.lib: $(OBJS)
	$(call lib,$@,$^)
