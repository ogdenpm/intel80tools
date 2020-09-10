ROOT=../..
REF=ref
TARGETS=tex10.com tex12.com tex21.com tex21a.com
PROTECT = build.ninja tex10.patch tex12.patch tex21.patch
include $(ROOT)/tools/isis.mk

PLMFLAGS=code optimize 
ASMFLAGS=

all::
	$(MAKE) $(TARGETS)

tex10.com: tex10.abs tex10.patch
	$(ROOT)/tools/obj2bin $^ $@

tex12.com: tex12.abs tex12.patch
	$(ROOT)/tools/obj2bin $^ $@


tex21.com: tex21.abs tex21.patch
	$(ROOT)/tools/obj2bin $^ $@

tex21a.com: tex21a.abs
	$(ROOT)/tools/obj2bin $^ $@

# intermediate files
.INTERMEDIATE: $(TARGETS:.com=.rel) $(TARGETS:.com=.abs)
       	       
STACK=100
tex10.abs: STACK=60

%.abs: %.rel
	$(call locate,$@,$^,code(100h) stacksize($(STACK)) purge)

tex10.rel: tex10.obj x0100.obj 
	$(call link,$@,$^ $(plm80.lib))

tex12.rel: tex12.obj cpm.obj
	$(call link,$@,$^ $(plm80.lib))

tex21.rel: tex21.obj cpmio.obj cpm.obj
	$(call link,$@,$^ $(plm80.lib))

tex21a.rel: tex21a.obj cpmio.obj cpm.obj
	$(call link,$@,$^ $(plm80.lib))

clean::
	rm -fr *.rel
