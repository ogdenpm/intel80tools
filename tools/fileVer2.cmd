@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
REM  Simplified version of version.cmd that shows the revisions to a specific file
REM  since creation.
REM  Limitation is that it does not track moves or renames (except case change)
::
:: Console output only
if /I "%~1" == "-v" (echo %0: Rev  8  -- b7d37a2 [2021-08-25]) & goto :EOF
if "%~1" == "-q" (
    set QUIET=1
    SHIFT /1
)
if [%1] == [] goto USAGE
if not exist "%~1" goto USAGE
::
:: find the containing directory of the file
set PATHTO=%~p1
:: get the directory as a filename by removing trailing \
for %%I in ("%PATHTO:~,-1%") do set PARENT=%%~nxI
set FILE=%~1
if "%~1" neq "%~nx1" set HASDIR=YES
set FILE="%~1"

REM ===================
REM Entry Point
REM ===================
:START
CALL :GET_VERSION_STRING

:: pretty print the information on a single line
set FILE=%~nx1
call :pad FILE 20
if [%GIT_SHA1%] == [] echo %FILE% Rev: untracked & goto :eof

set REVISION=%GIT_COMMITS%%GIT_QUALIFIER%

if [%GIT_BRANCH%] neq [master] if [%GIT_BRAHCN%] neq [main] set REVISION=%REVISION% {%GIT_BRANCH%}
call :pad "REVISION" 3

echo %FILE% Rev: %REVISION% -- git %GIT_SHA1% [%GIT_CTIME%]
exit /b 0

:: --------------------
:USAGE
:: --------------------
ECHO usage: fileVer [-v] ^| [-q] file
ECHO where -v shows version info
ECHO       -q supresses no Git info for file message 
GOTO :EOF



REM ====================
REM FUNCTIONS
REM ====================
:: --------------------
:GET_VERSION_STRING
:: --------------------

:: Get which branch we are on and whether any outstanding commits in current tree
for /f "tokens=1,2 delims=. " %%A in ('git status -s -b -uno -- ":(icase)%FILE%" 2^>NUL') do (
    if [%%A] == [##] (
        set GIT_BRANCH=%%B
    ) else (
        if [%%A%%B] neq [] (
            set GIT_QUALIFIER=+
            goto :gotQualifier
        )
    )
)
:gotQualifier
:: error then no git or not in a repo (ERRORLEVEL not reliable)
IF not defined GIT_BRANCH goto :EOF

:: get the current SHA1 and commit time for the file
for /f "tokens=1,2" %%A in ('git log -1 "--format=%%h %%ct" -- "%FILE%"') do (
    set GIT_SHA1=%%A
    call :gmTime GIT_CTIME %%B
)

:: to work out the revision number using git log --follow

for /f %%C in ('git log --follow --oneline -- "%FILE%" ^| find /v "" /c') do set GIT_COMMITS=%%C

goto :EOF


:: pad a field to a specified width
:pad str width
setlocal
set FIELD=!%~1!
set TWENTYSPACES=                    
call set TFIELD="!FIELD:~0,%~2!"
IF ["%FIELD%"] == [%TFIELD%] (
    set FIELD=%FIELD%%TWENTYSPACES%
    call set FIELD=!FIELD:~0,%~2!
)
endlocal & set %~1=%FIELD%
goto :EOF

:: convert unix time to gmt yyyy-mm-dd hh:mm:ss
:gmtime gmtstr utime
setlocal
set /a z=%2/86400+719468,d=z%%146097,y=^(d-d/1460+d/36525-d/146096^)/365,d-=365*y+y/4-y/100,m=^(5*d+2^)/153
set /a d-=^(153*m+2^)/5-1,y+=z/146097*400+m/11,m=^(m+2^)%%12+1
set /a h=%2/3600%%24,mi=%2%%3600/60,s=%2%%60
if %m% lss 10 set m=0%m%
if %d% lss 10 set d=0%d%
if %h% lss 10 set h=0%h%
if %mi% lss 10 set mi=0%mi%
if %s% lss 10 set s=0%s%
endlocal & set %1=%y%-%m%-%d% %h%:%mi%:%s%
goto :EOF
