# path to root of build tree
ROOT := ../..
TARGETS := as80.bin editor.bin
PEXFILE:=as80.pex

include $(ROOT)/tools/isis.mk

REF = ref
# build options
LOCATEFLAGS:=ORDER(DATA,STACK,MEMORY,CODE) SYMBOLS
PLMFLAGS:= DEBUG CODE
LINKFLAGS:=
ASPATCH=as80_1.1.patch
EDPATCH=editor_1.1.patch
PROTECT=$(ASPATCH) $(EDPATCH)

ASobjs = as80.obj pcktok.obj as81.obj as82.obj as83.obj\
	     as84.obj as85.obj as86.obj as87.obj as88.obj keywrd.obj

EDobjs = editor.obj dblank.obj dlimit.obj fupper.obj numout.obj\
	     seq.obj ucase.obj 

$(ASobjs): PLM80=3.1

all::
	$(MAKE) $(TARGETS)

as80.bin: as80.abs $(ASPATCH)
	$(OBJBIN) $^ $@

as80.abs: as80.rel
	$(call locate-overlaps,$@,$^,data(40h) code(0B800h) stacksize(82))

as80.rel: $(ASobjs)
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

editor.abs: editor.rel
	$(call locate-overlaps,$@,$^,DATA(2000h) CODE(0a800h))

editor.rel: $(EDobjs) 
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

editor.bin: editor.abs $(EDPATCH)
	$(OBJBIN) $^ $@
