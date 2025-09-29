@echo off
set F16=%1
set F32=%F16:.=_32.%
msdos32 -c%F32% %F16%
