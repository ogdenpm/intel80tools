system comes directly from TOS2.1 v1.18
system.omf was generated from system using

    abstool -a system system.omf

Note abstool loads the whole image in memory and writes out an optimised
omf file, with records in address order.

To convert back to a new ISIS bin file the command
    abstool -i system.omf system.bin

could be used, however it will save the records in address order. The original
system file contains some out of order records, and whilst generating the same
image when loaded, the two files will not be identical.
