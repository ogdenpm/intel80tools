# path to root of build tree
ROOT := ../..
TARGETS := locate

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG
LINKFLAGS:=

include $(ROOT)/tools/isis.mk

PEXFILE:=locate.pex

LOWOBJ=loc1.obj loc2a.obj memmov.obj loc2b.obj loc3.obj loc4.obj loc4a.obj loc5a.obj loc5b.obj isis.obj
HIGHOBJ=loc6.obj loc7.obj loc8.obj loc8a.obj loc9.obj

all:: $(TARGETS)

# locate is built in two parts the base address of the higher part
# is the MEMORY address at the end of the loctmp.map file

locate: loctmp.abs $(HIGHOBJ)
	$(call link,locate.rel,$^ $(call v40dir,system.lib plm80.lib))
	$(call locate,$@,locate.rel,STACKSIZE(0) CODE(6656h) PURGE)
	@rm locate.rel

# intermediate target - note will have unsatisfied externals see also HICODE above
loctmp.abs: $(LOWOBJ)
	$(call link-nocheck,loctmp.rel,$^ $(V40)/plm80.lib)
	$(call locate-nocheck,$@,loctmp.rel,STACKSIZE(6Ch))
	@rm loctmp.rel

