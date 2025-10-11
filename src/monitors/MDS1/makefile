TARGETS := b1n10.rom b1v10.rom b1v11.rom b1v12.rom m1n10.rom \
		   m1v11.rom m2v11.rom m2v12.rom
ITOOLS=../../..
include $(ITOOLS)/tools/isis.mk
ASMFLAGS=MACROFILE

all:: $(TARGETS)

%.obj: %.asm
	$(call asm80,$@,$<)


%.rom : %.obj
	$(ITOOLS)\tools\aomf2bin -z $< $@

distclean::
	rm -f *.rom
