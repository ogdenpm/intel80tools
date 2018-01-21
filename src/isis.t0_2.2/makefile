# path to root of build tree
ROOT := ../..
TARGETS := isis.t0
PROTECT := LoadAndFix.pl

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,2.2)


all:: $(TARGETS)

isis.t0: isist0.hex
	perl loadAndFix.pl

isist0.hex: isist0.plm
	echo >fort.1
	cp $^ fort.2
	$(ROOT)/tools/plm81
	mv fort.16 fort.4
	mv fort.17 fort.7
	mv fort.12 isist0.prn
	$(ROOT)/tools/plm82
	mv fort.17 isist0.hex
	cat fort.12 >>isist0.prn
	rm fort.*

clean::
	rm isist0.hex isist0.prn
