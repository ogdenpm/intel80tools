ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REF := ref

include $(ROOT)/tools/isis.mk
# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

# possible options to add
# LINKFLAGS - nothing MAP, PRINT are used, PUBLICS is best handled as part of filenames
# PLMFLAGS - XREF SYMBOLS DEBUG PAGELENGTH(n) PAGEWIDTH(n) IXREF NOOPTIMIZE NOPAGING
# 	   CODE, OBJECT and PRINT are used - WORKFILES unlikely to be needed
# ASMFLAGS - DEBUG PAGELENGTH(n) PAGEWIDTH(n) NOSYMBOLS XREF MACRODEBUG MOD85 NOPAGING TTY
# 	   OBJECT and PRINT are used
# LOCATEFLAGS - COLUMNS(n) SYMBOLS LINES PUBLICS ORDER(seq)
# 	      MAP and  PRINT are used and the rest are probably best handled in the rule
# 	      i.e. PURGE CODE(addr) DATA(addr) STACK(addr) MEMORY(addr) /common name/(addr)
# 	      //(addr) NAME(name) RESTART0 START(addr) STACKSIZE(value)
LINKFLAGS := 
PLMFLAGS := DEBUG
ASMFLAGS := DEBUG
# stack area allocated in the loc62.asm file
LOCATEFLAGS := SYMBOLS PUBLICS
OBJ = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all: $(TARGETS)

isis.t0: $(OBJ) $(JUNKFILE)
	$(call link,isist0.rel,$^ $(V40)/plm80.lib)
	$(call locate-overlaps,isist0.abs,isist0.rel,name(isist0) stacksize(42) code(3000h))
	@rm isist0.rel
ifdef JUNKFILE
	@echo "##################################################################"
	@echo "#### Memory overlaps are expected for address ranges:         ####"
	@echo "####    3AC9H-3AF2H, 3B0FH-3B12H, 3B1CH-3B1FH, 3B29H-3B2CH,   ####"
	@echo "####    3B32H-3B32H, 3B56H-3B56H, and 367H-3B7FH              ####"
	@echo "##################################################################"
endif
	$(ROOT)/tools/obj2bin isist0.abs $@
	@rm isist0.abs

