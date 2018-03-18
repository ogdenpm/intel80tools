# path to root of build tree
ROOT := ../..
TARGETS := isis.bin
PROTECT := LoadAndFix.pl

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,2.2)


all:: $(TARGETS)

isis.hex: isis.plm
	echo >fort.1
	cp $^ fort.2
	$(ROOT)/tools/plm81
	mv fort.16 fort.4
	mv fort.17 fort.7
	mv fort.12 isis.lst
	echo "\$$*=1 A=2 N=1 H=8" >fort.1
	echo " " >>fort.1
	$(ROOT)/tools/plm82
	mv fort.17 isis.hex
	cat fort.12 >>isis.lst
	perl $(ROOT)/pretty.pl isis.lst isis.prn
	rm fort.*

isis.bin: isis.hex
	perl loadAndFix.pl

clean::
	rm isis.hex isis.prn

distclean::
	rm isis.hex isis.prn
