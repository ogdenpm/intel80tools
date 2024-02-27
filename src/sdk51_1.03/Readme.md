# SDK51

This directory contains the source files to make the SDK51 ver 1.03 PROM image.

The work to OCR the original listing was done by Matti Nummi <matti.nummi@hotmail.fi>. His original work included modifying the code to assemble  with the AS assembler http://john.ccac.rwth-aachen.de:8000/as/.

I have modified the code to assemble with Intel's ASM51, fixing some OCR issues and splitting the include files into separate files. This modified version was then repackaged to again build with AS.

The main directory contains the code to build using ASM51, under the thames emulator, the file sdk51_1.03-as_version.zip, contains the corresponding code to build with AS.

Known differences from the source listing are:

- Recent versions of ASM51, treat NUMBER as a reserved word. The label NUMBER has been renamed _NUMBER to work around this. This is not an issue for AS.
- Recent versions of ASM51 do not generate hex files directly, so the command line in the listing is different.
- AS uses & and | in expressions rather than AND and OR
- AS does not support DBIT. These are replaced by the equivalent BIT instructions.
- For AS, a set of helper macros have been written (helper.inc) to support BSEG, CSEG, DSET, HIGH and LOW.
- AS optimises JMP better than ASM51, to work around this some JMPs have been replaced with explicit LJMP instructions

Both builds include makefiles for use by gnu make under windows. These support the targets all (default), clean, distclean, rebuild and  verify. The AS build assumes the assembler is installed correctly and  on your execution PATH.

The doc subdirectory contains the scanned pdf listing files and ref subdirectory contains the PROM images, where MONASM.BIN is the combined PROM image to make verification simpler.

Note, the zip file contains the PROM image as monasm.bin.ref, to make it a standalone package.

The zip file also containst zero.pad which is 15 NULL that are concatenated to the generated file to fill to the PROM image boundary. It is not needed for the ASM51 build as it uses an alternative technique to do this.

```
Updated 27-Feb-2024 Mark Ogden
```

