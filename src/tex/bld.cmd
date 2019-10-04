plm80 tex.plm code xref
asm80 x0100.asm code
link tex.obj,x0100.obj,plm80.lib to tex.mod
locate tex.mod code(0100H) stacksize(60)
objhex tex to tex.hex
exit
