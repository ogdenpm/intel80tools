These files are reconstructions of the source code from the PDF of the INSITE
AB22 submission

BIOS AND BOOT PROGRAMS FOR CP/M-80 RUNNUG ON iSBC-80/24 AND 218 BOARDS

In the order presented in the pdf they are
LDCPM.ASM   + LDCPM.LST     - assemble using INTEL ASM80
BIO115.ASM  + BIO115.LST    - assemble using INTEL ASM80
FORMAT.ASM  + FORMAT.PRN    - assemble using CP/M ASM
CPYDSK.ASM  + CPYDSK.PRN    - assemble using CP/M ASM

Benign differences
General
1) Although I have tried to match spacing it is possible that they don't
always match the original. Same applies for boxed comments.
2) As the letter O and digit 0 and the letter I and the digit 1 are difficult
to differentiate in the source code, it is possible that I have chosen the wrong
character. As the code assembles it appears that I have at least been
consistent.
3) Although I have scanned the comment text, it is possible I have not caught
all OCR errors.

LDCPM.ASM and BIO115.ASM
1) The command line doesn't match as I didn't mount the files on the same
drives

FORMAT.ASM
1) The best match I have found for the assembler used to produce the
FORMAT.PRN file was the CP/M ASM assembler. Unfortunately this complained
about the $EJECT controls, although the rest of the output matched. To avoid
the warning I have commented out the two $EJECT lines.

Mark 6-Dec-2024

