# path to root of build tree
ITOOLS ?= ../..
TARGETS := m81.com m81a.com l81.com l82.com l83.com l82fix.com l82a.com
PROTECT := readme.txt m81.patch l81.patch l82.patch l83.patch

include $(ITOOLS)/tools/isis.mk

REF = ref

all:: $(TARGETS)

%.com: %.hex
	$(call abstool,$@,$^)

%a.hex: %.hex
	cp $^ $@

l81.com: l81.hex vectab.abs
	$(call abstool,$@,$^)

vectab.abs: vectab.obj
	$(call link,vectab.rel,$^)
	$(call locate,$@,vectab.rel)
	rm vectab.rel

clean::
	rm -fr *.hex *.prn *.omf *.com l82fix.patch

.INTERMEDIATE: l82.hex l82a.hex 
.PRECIOUS:	%.hex 
