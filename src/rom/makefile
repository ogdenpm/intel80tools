ROOT := ../..
TARGETS := editor.bin

include $(ROOT)/tools/isis.mk
PATCH=editor_1.1.patch
PROTECT=$(PATCH)
PLMFLAGS=DEBUG CODE

REF = ref

OBJS = editor.obj dblank.obj dlimit.obj fupper.obj numout.obj\
	   seq.obj ucase.obj

all::
	$(MAKE) $(TARGETS)

editor.abs: editor.rel
	$(call locate-overlaps,$@,$^,ORDER(DATA,STACK,CODE) order(DATA,STACK,MEMORY,CODE) DATA(2000h) code(0a800h)) SYMBOLS

editor.rel: $(OBJS) 
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

editor.bin: editor.abs $(PATCH)
	$(OBJBIN) $^ $@
