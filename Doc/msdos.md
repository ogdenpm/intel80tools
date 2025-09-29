# Intel MSDOS tools

The msdos directory contains a number of Intel's MSDOS based development tools. As they are ancient they will not directly run under 64 bit windows.

The msdos directory contains an emulator which can be used to run one of files e.g.

```
msdos asm86 ....			64bit version
msdos32 asm86 ...			32bit version
```

Alternatively you can create versions of the old tools with msdos bound to them

```
mkrun32 file.exe		creates file_32.exe for a 32bit version
mkrun64 file.exe		creates file_64.exe for a 64bit version
```

 Alternatively you can run them in a fuller does emulation environment such as dosbox.

Note, I previously created bound versions and used them. This created more work when I wanted to use a later msdos emulator and also took up more github space. I now use the msdos emulator directly in my makefiles, but as noted you can create your own bound versions if desired.

The applications themselves appear to have been written initially for iRMX / ISIS IV and to make them available MSDOS, the Intel UDI2DOS utility was used to encapsulate the applications in a wrapper that mapped iRMX UDI calls to MSDOS.

**The iRMX and UDIDOS wrapper do bring a serious limitation that you need to be aware of, because not only do they force the MSDOS 8.3 filename limitations, they also enforce names to be alphanumeric. This means that you cannot use any long filenames in the file path, even if MSDOS can manage them by mapping to names with ~ in them.**

Another limitation is that temporary files have fixed names, which means that the applications cannot be used in parallel builds. In the original MSDOS scenario wasn't a problem. To address this if you are using make to control builds do either of

1. Put the phony target .NOTPARRALEL into the makefile. This is the preferred option.
2. Do not use the make -j option to use parallel builds

### 8086 tools

The following tools are provided

```
asm86	versions 2.1 and 3.2
lib86	version 2.1
link86	version 3.1
loc86	version 2.5
oh86	version 1.0
```

### 80286 tools

```
asm286	version 1.3
bld286	version 3.2
bnd286	version 3.2
lib286	version 3.2
map286	version 3.2
ovl286	version 3.2
```

### Misc Tools

```
ipps		version 2.3 Intel's prom programming software
pcpplod		version 3.0
msdos.exe	the msdos 16 bit emulation / wrapper for 64bit OS
msdos32.exe the msdos 16 bit emulation / wrapper for 32bit OS
The latest versions of the msdos emulator can be found at
http://takeda-toshiya.my.coocan.jp/msdos/index.html
Where you can also obtain the source code and prebuild versios for emulating later CPUs.
```

------

```
Updated by Mark Ogden 29-Sep-2025
```