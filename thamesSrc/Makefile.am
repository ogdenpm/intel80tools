## Process this file with automake to produce Makefile.in

SUBDIRS= . bin 
EXTRA_DIST= $(srcdir)/config/* $(srcdir)/thames.txt $(srcdir)/thames.html \
	INSTALL COPYING ChangeLog
pkgdata_DATA = thames.txt

ACLOCAL_AMFLAGS=-I m4

thames.txt:	$(srcdir)/thames.html
	lynx -dump $(srcdir)/thames.html > thames.txt
