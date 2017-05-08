# common makefile info
ROOT=../..
TARGETS=plm80.lib
PLMFLAGS=code optimize
ASMFLAGS=
include $(ROOT)/tools/isis.mk

REF=$(call ipath,plm80.lib)

OBJS=	p0000.obj p0002.obj p0004.obj p0007.obj p0011.obj p0014.obj \
	p0018.obj p0020.obj p0022.obj p0025.obj p0029.obj p0031.obj \
	p0034.obj p0036.obj p0039.obj p0042.obj p0045.obj p0048.obj \
	p0050.obj p0052.obj p0055.obj p0059.obj p0061.obj p0064.obj \
	p0066.obj p0069.obj p0071.obj p0073.obj p0075.obj p0077.obj \
	p0080.obj p0082.obj p0084.obj p0086.obj p0089.obj p0091.obj \
	p0094.obj p0096.obj p0098.obj p0101.obj p0103.obj p0105.obj \
	p0106.obj p0108.obj p0110.obj p0113.obj prmsk.obj psmsk.obj

all:: $(TARGETS)

plm80.lib: $(OBJS)
	$(call lib,$@,$^)

