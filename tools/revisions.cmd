@echo off
setlocal

if /I "%~1" == "-v" (echo %0: Rev 3+ -- git d9362bd [2020-10-10]) & goto :EOF

:doOpt
if [%1] == [] goto execute
if [%1] == [-q] (set QUIET=%1) else if [%1] == [-s] (set SUBDIR=1) else goto usage
SHIFT /1
goto doOpt

:usage
echo usage: %0 [-v] ^| [-r] [-q]
echo where -v shows version info
echo       -s also shows files in immediate sub-directories
echo       -q supresses no Git info for file message
goto :eof

:execute

for %%I in (*) do if /I [%%~xI] neq [.exe] call filever %QUIET% "%%I"

if not defined SUBDIR goto :eof

for /D %%D in (*.*) do (
    if [%%~nD] neq [] (
        echo %%D\
        for %%I in (%%D\*) do if /I [%%~xI] neq [.exe] call filever %QUIET% "%%I"
    )
)


