ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
TOOLS=../../tools/
F0=./
F2=.\list

PEXFILE=asm80.pex
PLMFLAGS=code print(:f2:$*.lst)

OVERLAY=6D4Ch

# bulk obj names
BASEOBJS = glb.obj asm801.obj asm82.obj asm83.obj asm46.obj asm85.obj\
	asm851.obj asm86.obj asm861.obj pcktkn.obj close.obj delete.obj error.obj\
       	exit.obj load.obj open.obj read.obj rescan.obj seek.obj write.obj

OBJS4 =	glb4.obj asm41.obj pcktkn.obj asm43.obj asm44.obj asm45.obj asm46.obj\
	asm47.obj asm48.obj asm49.obj asm4A.obj asm4B.obj asm4C.obj asm4D.obj asm4E.obj\
	close.obj delete.obj error.obj exit.obj\
	load.obj open.obj read.obj rescan.obj\
	seek.obj write.obj

OBJS5 =	glb5.obj asm51.obj pcktkn.obj asm82.obj asm83.obj asm55.obj asm46.obj\
	asm57.obj asm86.obj asm861.obj asmov0.obj asmov2.obj asmov1.obj asm5D.obj\
	close.obj delete.obj error.obj exit.obj\
	load.obj open.obj read.obj rescan.obj\
	seek.obj write.obj

P4SRCS = asm41.plm asm44.plm asm45.plm asm4e.plm asm51.plm asm55.plm asm5d.plm\
	asm801.plm asm83.plm asm851.plm asm861.plm asmov3.plm close.plm delete.plm\
	error.plm exit.plm glb.plm glb4.plm glb5.plm key4.plm keywrd.plm load.plm\
	open.plm read.plm rescan.plm seek.plm write.plm

P3SRCS = asm43.pl3 asm46.pl3 asm47.pl3 asm48.pl3 asm49.pl3 asm4a.pl3 asm4B.pl3 asm4c.pl3\
	asm4d.pl3 asm57.pl3 asm82.pl3 asm85.pl3 asm86.pl3 asmov0.pl3 asmov1.pl3 asmov2.pl3

ASRCS = pcktkn.asm cprite.asm memory.asm

TARGETS = asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5

#
# compilation and assembly rules
.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@find $(F2)\$*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.plm.ipx:
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)ngenpex $(PEXFILE) $<

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)ngenpex $(PEXFILE) $<
	
.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:ASM80 $< PRINT(:f2:$*.lst)
	@find $(F2)\$*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)

# the default rule will force the makefile to be up to date
all: source

all:
	$(MAKE) $(TARGETS)

rebuild: uclean all

.extract: asm80_all.plm
	..\..\unpack.pl
	date /t >.extract


source: .extract $(ASRCS) $(P3SRCS) $(P4SRCS) $(P3SRCS:.pl3=.ipx) $(P4SRCS:.plm=.ipx)

# checking targets
# note if you change these two rules then you should do a nmake all or namke .extract first
# otherwise whilst the build will be ok the old verify/compare rules will be run

verify: all
	for %i in ($(TARGETS)) do $(TOOLS:/=\)diffbin %i $(V4)\%i

compare: all
	for %i in ($(TARGETS)) do fc /b %i $(V4)%i


# source files generated from shared code
asm43.pl3 asm82.pl3: asm4382.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm43.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm82.pl3 $**

asm48.pl3 asm86.pl3: asm4886.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm48.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm86.pl3 $**

asm49.pl3 asm861.plm: asm49861.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm49.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm861.plm $**

asm44.plm asm83.plm: as4483.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm44.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o asm83.plm $**

asm45.plm asm55.plm asm851.plm: as4555851.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm45.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm55.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o asm851.plm $**

asm4a.pl3 asmov0.pl3: asm4aov0.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4a.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL0 -o asmov0.pl3 $**

asm4d.pl3 asmov1.pl3: asm4dov1.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4d.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL1 -o asmov1.pl3 $**

asm4c.pl3 asmov2.pl3: asm4cov2.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4c.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL2 -o asmov2.pl3 $**

asm47.pl3 asm57.pl3 asm85.pl3: asm475785.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm47.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm57.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm85.pl3 $**

asm41.plm asm51.plm asm801.plm: start.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm41.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm51.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o asm801.plm $**

glb.plm glb4.plm glb5.plm: global.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o glb4.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o glb5.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o glb.plm $**

asm4e.plm asm5d.plm asmov3.plm: init.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4e.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm5d.plm $**
	$(TOOLS:/=\)plmpp -sOVL3 -o asmov3.plm $**

# build rules to make the program and overlays

asm80: base.abs asmov0.abs asmov1.abs asmov2.abs asmov3.abs keywrd.abs 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link base.abs,publics(asmov0.abs,asmov1.abs,asmov2.abs), "&" < <<
asmov3.abs,keywrd.abs to $*.rel &
map print(:f2:$*.lin)
<<NOKEEP
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS print(:f2:$*.map) "&" < <<
STACKSIZE(0) PURGE NAME(ASM80)
exit
<<NOKEEP
	del $*.rel
	@find $(F2)\$*.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\$*.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asm80.ov0: asmov0.abs 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:locate $** to $@ SYMBOLS MAP PUBLICS print(:f2:asmov0.map) "&" < <<
STACKSIZE(0) PURGE NAME(A80O0)
<<NOKEEP
	@(find $(F2)\$*.mo0 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asm80.ov1: asmov1.abs 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:locate $** to $@ SYMBOLS MAP PUBLICS print(:f2:asmov1.map) "&" < <<
STACKSIZE(0) PURGE NAME(A80O1)
<<NOKEEP	
	@(find $(F2)\$*.mo1 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


asm80.ov2: asmov2.abs 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:locate $** to $@ SYMBOLS MAP PUBLICS print(:f2:asmov2.map) "&" < <<
STACKSIZE(0) PURGE NAME(A80O2)
<<NOKEEP	
	@(find $(F2)\$*.mo2 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asm80.ov3: asmov3.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:locate $** to $@ SYMBOLS MAP PUBLICS print(:f2:asmov3.map) "&" < <<
STACKSIZE(0) PURGE NAME(A80O3)
<<NOKEEP
	@(find $(F2)\$*.mo3 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


asm80.ov4: $(OBJS4) key4.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
$(OBJS4: =, & 
),:f1:plm80.lib,key4.obj to asmov4.rel map print(:f2:asmov4.lin)
<<NOKEEP
	$(ISIS) :F1:locate asmov4.rel to $@ SYMBOLS NAME(A80O4) PURGE "&" < <<
MAP PUBLICS code(3780h) STACKSIZE(144) print(:f2:asmov4.map)
exit
<<NOKEEP
	@find $(F2)\asmov4.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov4.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


asm80.ov5: $(OBJS5) keywrd.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
$(OBJS5: =, &
),:f1:plm80.lib,keywrd.obj to asmov5.rel map print(:f2:asmov5.lin)
<<NOKEEP
	$(ISIS) :F1:locate asmov5.rel to $@ SYMBOLS NAME(A80O5) PURGE "&" < <<
MAP PUBLICS code(3780h) STACKSIZE(144) print(:f2:asmov5.map)
exit
<<NOKEEP
	@find $(F2)\asmov5.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov5.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)



# rules for intermediate files

base.abs: $(BASEOBJS)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
$(BASEOBJS: =, & 
),:f1:plm80.lib to $*.rel print(:f2:$*.lin)
<<NOKEEP
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS STACKSIZE(120) print(:f2:$*.map)
	del $*.rel
	@find $(F2)\$*.lin "UNRESOLVED EXTERNAL" >nul || (del $@ & exit /b 1)


keywrd.abs: keywrd.obj memory.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link $(**: =,) to $*.rel map print(:f2:$*.lin)
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE(075A2h) STACKSIZE(0) print(:f2:$*.map)
	del $*.rel
	@find $(F2)\$*.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\$*.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asmov0.abs: base.abs cprite.obj asmov0.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov0.obj to $*.rel map print(:f2:$*.la0)
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$*.ma0)
	del $*.rel
	@find $(F2)\$*.la0 "LINK MAP" >nul || (del $@ & exit /b 1)


asmov1.abs: base.abs cprite.obj asmov1.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov1.obj,:F1:plm80.lib "&" < <<
to $*.rel map print(:f2:$*.la1)
<<NOKEEP
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$*.ma1)
	del $*.rel
	@find $(F2)\$*.la1 "LINK MAP" >nul || (del $@ & exit /b 1)


asmov2.abs: base.abs cprite.obj asmov2.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov2.obj to $*.rel map print(:f2:$*.la2)
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$*.ma2)
	del $*.rel
	@find $(F2)\$*.la2 "LINK MAP" >nul || (del $@ & exit /b 1)


asmov3.abs: base.abs cprite.obj asmov3.obj keywrd.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov3.obj,publics(keywrd.abs) "&" < <<
to $*.rel map print(:f2:$*.la3)
<<NOKEEP
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$*.ma3)
	del $*.rel
	@find $(F2)\$*.la3 "LINK MAP" >nul || (del $@ & exit /b 1)

# pex file and makefile dependencies
$(P3SRCS:.pl3=.ipx) $(P3SRCS:.pl3=.obj): $(PEXFILE) makefile
$(P4SRCS:.plm=.ipx) $(P4SRCS:.plm=.obj): $(PEXFILE) makefile
$(ASRCS:.asm=.obj): makefile 

# housekeeping rules
clean:
	del /q *.obj *.abs *.rel *.*~ *~ *.ipx *.*~ 
	del /q $(F2)\*.*~ $(F2)\*.???

vclean: clean
	del asm80 asm80.ov?

uclean: vclean
	del ??????.* makefile *.plx .extract

gitprep: uclean
	..\..\unpack.pl



# auto generated

