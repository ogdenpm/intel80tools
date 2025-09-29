@echo off
set F16=%1
set F64=%F16:.=_64.%
msdos -c%F64% %F16%
