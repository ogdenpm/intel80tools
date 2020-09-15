# Intel80Tools

This repository contains my attempts at reverse engineering a number of Intel and CP/M tools and applications back into PL/M and assembler source code. My goal was to make sure that the reverse engineered source can be rebuilt to create a byte level match to the original binary images. With a few exceptions I have managed to achieve this, or in some cases create equivalent code, i.e. image loaded into memory will be the same. There are a very small number of cases, where it appears that internal / pre-release development tools were used, in these cases I have done my best to match the original or at least understand the issue.

In addition to my reverse engineering efforts, the repository contains the development tools I use, with the exception of Visual Studio and perl, both of which can be freely downloaded. Most of the bespoke tools I have used have source code in the repository, in addition to the binary files. One of the key tools is an enhanced version of John Elliott's thames emulator for ISIS. I have added several features to make it more user friendly, especially for use in development.

## News

The core documentation for the work is now located in the doc directory, where you will find an overview document along with more targeted documentation for various parts of the repository.

A recent addition to the repository is ISIS v1.1 (16k ISIS). I am not aware of this being elsewhere on the internet. It was uncovered in early September 2020 by Jon Hales a volunteer at the Cambridge Centre for Computing History, who found it in the centre's archives. In addition to the binary images for this rare version of ISIS, I have also reversed engineered all of the OS and applications, except for the assembler as80 which remains a work in progress.

------

```
Updated by Mark Ogden 15-Sep-2020
```

