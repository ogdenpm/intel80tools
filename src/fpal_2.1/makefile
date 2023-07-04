# common makefile info
ITOOLS ?=../..
TARGETS=fpal.lib
PLMFLAGS=code optimize
ASMFLAGS=
include $(ITOOLS)/tools/isis.mk

REF=$(call ipath,fpal.lib,2.1)
PLM80=3.1
#ASM80=2.0

OBJS= cpyrit.obj v2p1.obj check.obj fabs.obj fadd.obj fqfb2d.obj fclr.obj \
	  fcmpr.obj fqfd2b.obj fdiv.obj ferhnd.obj ferror.obj fixsd.obj fload.obj \
	  fltds.obj fmul.obj fneg.obj freset.obj fset.obj fsqrt.obj fstat.obj \
	  fstor.obj fztst.obj mply.obj norml.obj fqft10.obj fqfx10.obj \
	  round.obj shift.obj

all::
	$(MAKE) $(TARGETS)

fpal.lib: $(OBJS)
	$(call lib,$@,$^)

