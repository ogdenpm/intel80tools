ROOT := ../..
TARGETS = isis.t0
PEXFILE := isist0.pex
REFDIR := ref

include $(ROOT)/tools/isistools.mk
# comment out below to omit junk filling - note verify will fail
JUNKFILE := junk.obj

# possible options to add
# LINKOPT - nothing MAP, PRINT are used, PUBLICS is best handled as part of filenames
# PLMOPT - XREF SYMBOLS DEBUG PAGELENGTH(n) PAGEWIDTH(n) IXREF NOOPTIMIZE NOPAGING
# 	   CODE, OBJECT and PRINT are used - WORKFILES unlikely to be needed
# ASMOPT - DEBUG PAGELENGTH(n) PAGEWIDTH(n) NOSYMBOLS XREF MACRODEBUG MOD85 NOPAGING TTY
# 	   OBJECT and PRINT are used
# LOCATEOPT - COLUMNS(n) SYMBOLS LINES PUBLICS ORDER(seq)
# 	      MAP and  PRINT are used and the rest are probably best handled in the rule
# 	      i.e. PURGE CODE(addr) DATA(addr) STACK(addr) MEMORY(addr) /common name/(addr)
# 	      //(addr) NAME(name) RESTART0 START(addr) STACKSIZE(value)
LINKOPT := 
PLMOPT := DEBUG
ASMOPT := DEBUG
# stack area allocated in the loc62.asm file
LOCATEOPT := SYMBOLS PUBLICS
OBJ = boot.obj t0boot.obj consol.obj exit.obj memchk.obj write.obj

all: $(TARGETS)

isis.t0: $(OBJ) $(JUNKFILE)
	$(call link,isist0.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,isist0.abs,isist0.rel,name(isist0) stacksize(42) code(3000h))
	@del isist0.rel
ifdef JUNKFILE
	@echo ##################################################################
	@echo #### Memory overlaps are expected for address ranges:         ####
	@echo #### 3AC9H-3AF2H, 3B0FH-3B12H, 3B1CH-3B1FH, 3B29H-3B2CH,      ####
	@echo #### 3B32H-3B32H, 3B56H-3B56H, and 367H-3B7FH                 ####
	@echo ##################################################################
endif
	$(ROOT)/tools/obj2bin isist0.abs $@
	@del isist0.abs

