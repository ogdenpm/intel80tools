ROOT=../..
TARGETS=system.lib
PLMFLAGS=code optimize debug
ASMFLAGS=debug

include $(ROOT)/tools/isis.mk

REF=$(call ipath,utils,2.2)

PLM80 := 3.1

OBJS= 	attrib.obj ci.obj close.obj co.obj consol.obj csts.obj delete.obj exit.obj\
	iochk.obj iodef.obj ioset.obj isis.obj lo.obj load.obj memck.obj open.obj\
	po.obj read.obj rename.obj rescan.obj ri.obj seek.obj whocon.obj write.obj\
	ui.obj uo.obj upps.obj spath.obj error.obj getatt.obj getd.obj\
	filinf.obj chgacs.obj detime.obj v1p5.obj

all:: $(TARGETS)

system.lib: $(OBJS)
	$(call lib,$@,$^)
