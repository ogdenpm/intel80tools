# common makefile info
ROOT=../..
REF=ref
TARGETS=tex10.com tex12.com tex21.com tex21a.com

include $(ROOT)/tools/isis.mk

PLMFLAGS=code optimize 
ASMFLAGS=

all: $(TARGETS)

tex10.com: tex10.obj x0100.obj 
	$(call link,tex10.rel,$^ $(V40)/plm80.lib)
	$(call locate,tex10.abs,tex10.rel,code(100h) stacksize(60) purge)
	@rm tex10.rel
	$(ROOT)/tools/obj2bin tex10.abs $@
	@rm tex10.abs
	$(ROOT)/tools/patchbin patch10.txt $@

tex12.com: tex12.obj cpm.obj
	$(call link,tex12.rel,$^ $(V40)/plm80.lib)
	$(call locate,tex12.abs,tex12.rel,code(100h) stacksize(100) purge)
	@rm tex12.rel
	$(ROOT)/tools/obj2bin tex12.abs $@
	@rm tex12.abs


tex21.com: tex21.obj cpmio.obj cpm.obj
	$(call link,tex21.rel,$^ $(V40)/plm80.lib)
	$(call locate,tex21.abs,tex21.rel,code(100h) stacksize(100) purge)
	@rm tex21.rel
	$(ROOT)/tools/obj2bin tex21.abs $@
	@rm tex21.abs
	$(ROOT)/tools/patchbin patch21.txt $@

tex21a.com: tex21a.obj cpmio.obj cpm.obj
	$(call link,tex21a.rel,$^ $(V40)/plm80.lib)
	$(call locate,tex21a.abs,tex21a.rel,code(100h) stacksize(100) purge)
	@rm tex21a.rel
	$(ROOT)/tools/obj2bin tex21a.abs $@
	@rm tex21a.abs

