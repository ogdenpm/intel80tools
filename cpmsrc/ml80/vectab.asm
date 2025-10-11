    ASEG
    org 1C00H
LRACT:
    ds  1271 * 2
LRPACT:
    ds  255 * 2
LRR1:
    ds  160
LRR2:
    ds  160
LRPGO:
    ds  63
LRGO:
    ds 236
RESWRD:
    ds  333
SIZE equ $ - RESWRD

    org 2C00H
vectab:
    dw LRACT
	dw LRPACT
	dw LRR1
	dw LRR2
	dw LRGO
	dw LRPGO
	dw RESWRD
	dw SIZE

    end
