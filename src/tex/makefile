# common makefile info
ROOT=../..
REF=ref
TARGETS=tex10.com tex12.com tex21.com tex21a.com

include $(ROOT)/tools/isis.mk

PLMFLAGS=code optimize 
ASMFLAGS=
PURGE=purge
all:: $(TARGETS)

tex10.com: tex10.abs
	$(ROOT)/tools/obj2bin $^ $@
	$(ROOT)/tools/patchbin patch10.txt $@

tex12.com: tex12.abs
	$(ROOT)/tools/obj2bin $^ $@


tex21.com: tex21.abs
	$(ROOT)/tools/obj2bin $^ $@
	$(ROOT)/tools/patchbin patch21.txt $@

tex21a.com: tex21a.abs
	$(ROOT)/tools/obj2bin $^ $@

# intermediate files
.INTERMEDIATE: $(TARGETS:.com=.rel) $(TARGETS:.com=.abs)
       	       
STACK=100
tex10.abs: STACK=60

%.abs: %.rel
	$(call locate,$@,$^,code(100h) stacksize($(STACK)) $(PURGE))

tex10.rel: tex10.obj x0100.obj 
	$(call link,$@,$^ $(V40)/plm80.lib)

tex12.rel: tex12.obj cpm.obj
	$(call link,$@,$^ $(V40)/plm80.lib)

tex21.rel: tex21.obj cpmio.obj cpm.obj
	$(call link,$@,$^ $(V40)/plm80.lib)

tex21a.rel: tex21a.obj cpmio.obj cpm.obj
	$(call link,$@,$^ $(V40)/plm80.lib)
