@echo off
setlocal
rem locally install applications

if [%1] == [] goto usage
if [%2] neq [] goto start
:usage
echo usage: install file_with_path installRoot [configFile]
echo        configFile defaults to installRoot\install.cfg
echo.
echo install.cfg contains lines of the form type,dir[,suffix]
echo   Where type immediate parent directory name of the file to copy
echo   dir is the directory to install to; a leading + is replaced by installRoot
echo   suffix is inserted into the installed filename just before the .exe extension
echo   In both dir & suffix a $d is replaced by the current local date string in format yyyymmdd
echo   and a $t is replaced by the current local time string in format hhmmss
echo   All lines where type matches the input file's directory name are processed
echo.
echo Example with install.cfg in the current directory containing the line
echo x86-Release,+prebuilt
echo x86-Release,d:\bin,_32
echo.
echo install . path\x86-Release\myfile.exe
echo copies myfile to .\prebuilt\myfile.exe and d:\bin\myfile_32.exe
exit /b 1

:start

:: find the path to the target file
for %%I in ("%1") do set PATHTO=%%~pI
:: get the directory as a filename by removing trailing \
for %%I in ("%PATHTO:~,-1%") do set TYPE=%%~nxI

if [%3] == [] (
    set CONFIGFILE=%2\install.cfg
) else (
    set CONFIGFILE=%3
)
set CONFIGFILE=%CONFIGFILE:\\=\%

if not exist %CONFIGFILE% (
    echo cannot find %CONFIGFILE%
    echo.
    goto :error
)
:: get the current time for date/time modified dir or suffix
for /f "tokens=2 delims==." %%A in ('wmic os get LocalDateTime /format:list') do set NOW=%%A
:: process the intall config file
for /f "tokens=1,2,* delims=," %%A in (%CONFIGFILE%) do (
    if /I [%TYPE%] == [%%A] (
        call :copy "%1" "%2" "%%B" "%%C"
    )
)
goto :eof

:: the core code to copy the file to the desired location
:copy root src dir suffix
setlocal enabledelayedexpansion
:: work out install directory
:: replacing + with installRoot, replacing any \\ with \ and remove any trailing \
set DIR=%~3
:: substitute any date / time
call set DIR=!DIR:$d=%NOW:~,8%!
call set DIR=!DIR:$t=%NOW:~8,6%!

if [%DIR:~0,1%]==[+] set DIR=%~2\%DIR:~1%
set DIR=%DIR:\\=\%
if [%DIR:~-1%] == [\] set DIR=%DIR:~0,-1%
:: make sure we have a directory
if not exist %DIR%\NUL mkdir %DIR%
set SUFFIX=%~4
if [%SUFFIX%] neq [] (
:: map @ to local time in suffix
    call set SUFFIX=!SUFFIX:$d=%NOW:~,8%!
    call set SUFFIX=!SUFFIX:$t=%NOW:~8,6%!
)
:: finalise the filename to use, adding in any suffix
set FILE=%DIR%\%~n1%SUFFIX%%~x1
echo Installing %~1 to %FILE%
copy /b /y "%~1" "%FILE%"
goto :eof

:error

