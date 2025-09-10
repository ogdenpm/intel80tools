# shared makefile configuration for isis tool builds
ITOOLS ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
include $(ITOOLS)/tools/common.mk

# ISIS tools not applicable to CPM
ASM48 ?= 4.2
ASM51 ?= 2.1
PLM86 ?= 2.1
ASM86 ?= 2.1
LINK86 ?= 1.3
LOC86 ?= 1.3
ASM80X:=perl $(ITOOLS)/tools/asm80x.pl

PLM86FLAGS ?= code

# $(call asm80x,objfile,srcfile[,target specific options])
# asm80x.pl is a wrapper around asm80 to provide support for long variable
# names, the lst file and the obj file are also modified to make sure the
# long variable names are reflected in these..
# under the covers only asm80 v4.1 is used
# NOTE: unless structure support is needed use the c-port of asm80 v4.1
# as this now supports long names directly
define asm80x
  @$(ASM80X) $2 "object($1)" $(if $(ASMFLAGS), "$(ASMFLAGS)")$(if $3, "$3")
endef

# $(call asm48,objfile,srcfile[,target specific options])
define asm48
  @$(ISIS) $(call ifile,asm48,$(ASM48)) $2 "print($(call mkname,$2,lst))"\
	  "object($1)"$(if $(ASM48FLAGS), "$(ASM48FLAGS)")$(if $3, "$3")
endef

# $(call asm51,objfile,srcfile[,target specific options])
define asm51
  $(ISIS) $(call ifile,asm51,$(ASM51)) $2 "print($(call mkname,$2,lst))"\
	  "object($1)"$(if $(ASM51FLAGS), "$(ASM51FLAGS)")$(if $3, "$3")
endef

# $(call plm86,objfile,srcfile[,target specific options])
define plm86
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2)
  @$(ISIS) $(if $(PEXFILE),,-MF .deps/$1.d) $(call ifile,plm86,$(PLM86)) $2 "object($1)"\
	  $(if $(PLM86FLAGS), "$(PLM86FLAGS)")$(if $3, "$3")
endef

# $(call asm86,objfile,srcfile[,target specific options])
#
define asm86
  @$(ISIS) $(call ifile,asm86,$(ASM86)) $2 \
	  "object($1)"$(if $(ASM86FLAGS), "$(ASM86FLAGS)")$(if $3, "$3")
endef


$(OBJ)/%.obj: %.asmx | $(OBJ) $(LST)
	$(call asm80x,$@,$<)

$(OBJ)/%.hex: %.a48 | $(OBJ) $(LST)
	$(call asm48,$@,$<)

$(OBJ)/%.o86: %.plm | $(OBJ) $(LST)
	$(call plm86,$@,$<)

$(OBJ)/%.o86: %.asm  | $(OBJ) $(LST)
	$(call asm86,$@,$<)



