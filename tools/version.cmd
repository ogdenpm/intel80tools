@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
REM  Script for generation of version info suitable for windows VERSION_INFO and application use
REM  Some of the ideas/implementaiton is based on GIT-VERSION-GEN.bat
REM  see https://github.com/Thell/git-vs-versioninfo-gen/blob/master/GIT-VS-VERSION-GEN.bat
REM
REM  Using git and a tracked file %DEFAULTS_FILE% and using user tags of the form [appid-]Major.Minor
REM  the following values are generated.  appid comes from the file %DEFAULTS_FILE% if it exists
REM  it may be null
REM
REM  GIT_VERSION: Major.Minor.Commits [qualifier]       where Major and Minor come from a user tag of the format
REM                                                     where qualifier is [.P | .X] [-branch] 
REM                                                       .P - using uncommited files
REM                                                       .X - externally tracked i.e. no git
REM                                                       -branch is present if branch is not master or main
REM  GIT_BUILDTYPE n                                    where n is
REM                                                     0 for normal build on master/main branch
REM                                                     1 for normal build not on master/main branch
REM                                                     2 for build using uncommited files
REM                                                     3 for build using untracked files 
REM  Note a windows resource version is also generated as Major,Minor,Commits,buildtype
REM  The tag used is chosen as highest parent version tag applicable to the current directory
REM  ** For some scenarios this may not always be the desired outcome, but for my usage it is fine **
REM  Commits is the count of commit applicable to this directory since the tag or when git was first used if no tag
REM  GIT_APPID:   Application name from file %DEFAULTS_FILE%  - omitted if no prefix
REM  GIT_SHA1:    SHA1 value of current commit or 'untracked' if outside git
REM  GIT_CTIME:   DATE & TIME of current commit (in UTC)
REM  GIT_CYEAR:   Year of current commit (useful for Copyright)
REM  GIT_APPNAME: Application name defaults to parent directory
REM
REM  All generated values are strings, except for the windows resource value GIT_VERSION_RC
REM
REM  if git is not installed or applicable and %DEFAULTS_FILE% file exists, some of the information is extracted
REM  from this file. This is the only  instance where the qualifier '.X' is used

set DEFAULTS_FILE=version.in

:: --------------------
:CHECK_ARGS
:: --------------------
set SCRIPTNAME=%0
:: Console output only
IF [%1] == [] GOTO START

if /I [%~1] == [-v] (echo %0: Rev 12 -- git 72ca9af [2020-10-12]) & goto :eof
IF "%~1" == "-h" GOTO USAGE
:optloop
IF "%~1" == "-q" SET fQUIET=1& SHIFT & goto :optloop
IF "%~1" == "-f" SET fFORCE=1& SHIFT & goto :optloop
if "%~1" neq "-a" (goto :endopt) else if [%~2] == [] goto USAGE
set GIT_APPID=%~2
SHIFT & SHIFT & goto :optloop

:endopt
if [%~1] == [] if defined fQUIET (goto USAGE) else goto START
set CACHE_DIR=%~df1\
if [%~2] == [] goto USAGE
set HEADER_OUT_FILE=%~df2
set HEADER_DIR=%~dp2
IF /I [%~x2] neq [.cs] SET STDHDR=1
if [%3] neq [] goto USAGE

:: above CACHE_DIR is forced to have a trailing \, fixup if now \\
set CACHE_DIR=%CACHE_DIR:\\=\%
:: make sure directory exists
if not exist %CACHE_DIR% (
    mkdir %CACHE_DIR%
    if not exist %CACHE_DIR% goto USAGE
)
SET CACHE_FILE=%CACHE_DIR%GIT_VERSION_INFO

:: make sure the HEADER_OUT_FILE directory exists
set HEADER_DIR=%HEADER_DIR:\\=\%
if not exist %HEADER_DIR% (
    mkdir %HEADER_DIR%
    if not exist %HEADER_DIR% goto USAGE
)
:: for some reason doing the inverse test fails!!!
:: check we aren't trying to write to a directory rather than a file
if not exist %HEADER_OUT_FILE%\ goto START

:: --------------------
:USAGE
:: --------------------
ECHO usage: %SCRIPTNAME% [-h] ^| [-q] [-f] [-a appid] [CACHE_PATH OUT_FILE]
ECHO.
ECHO  When called without arguments version information writes to console
ECHO.
ECHO  -h          - displays this output
ECHO.
ECHO  -q          - Suppress console output
ECHO  -f          - Ignore cached version information
ECHO  -a appid    - set appid. An appid of . is replaced by parent directory name
ECHO  CACHE_PATH  - Path for non-tracked file to store git version info used
ECHO  OUT_FILE    - Path to writable file where the generated information is saved
ECHO.
ECHO  Example pre-build event:
ECHO  CALL $(SolutionDir)scipts\version.cmd "Generated" "Generated\version.h"
ECHO.
ECHO  If OUT_FILE ends in .cs then C# version file is written else C/C++ include file
ECHO.
GOTO :EOF


REM ===================
REM Entry Point
REM ===================
:START
CALL :LOAD_DEFAULTS
CALL :GET_VERSION_STRING
if [%GIT_SHA1%] == [] (
    if [%defGIT_SHA1%] == [] (
        echo No Git information and no "%DEFAULTS_FILE%" file
        exit /b 1
    ) else (
        if defined STDHDR (
            echo copying %DEFAULTS_FILE% to %HEADER_OUT_FILE%
            copy /y/b "%DEFAULTS_FILE%" "%HEADER_OUT_FILE%" & exit /b 0
        )
        set GIT_SHA1=untracked
        set GIT_VERSION=%defGIT_VERSION%
        set GIT_BUILDTYPE=3
        set GIT_BRANCH=%defGIT_BRANCH%
        set GIT_CTIME=%defGIT_CTIME%
    )
)
if DEFINED CACHE_FILE CALL :CHECK_CACHE
if DEFINED fDONE exit /b 0
call :WRITE_OUT
exit /b 0



REM ====================
REM FUNCTIONS
REM ====================
:: --------------------
:LOAD_DEFAULTS
:: --------------------
:: load the defaults from the %DEFAULTS_FILE% file
:: line #define VAR VALUE gets a new variable defVAR with the VALUE (quotes are removed)
if exist %DEFAULTS_FILE% (
    for /f "tokens=1,2,3,4" %%A in (%DEFAULTS_FILE%) do if [%%A] == [#define] (call :setVar %%B %%C %%D)
)
:: Application name defaults to parent directory
for %%I in (%CD%) do set GIT_APPDIR=%%~nxI
:: if verison.rc had GIT_APPNAME use it else define as GIT_APPDIR
if defined defGIT_APPNAME (set GIT_APPNAME=%defGIT_APPNAME%) ELSE (set GIT_APPNAME=%GIT_APPDIR%)
:: user didn't specific APPID then use the one in version.rc
if not defined GIT_APPID if defined defGIT_APPID set GIT_APPID=%defGIT_APPID%
:: replace a name of . with the parent directory name
if [%GIT_APPID%] == [.] set GIT_APPID=%GIT_APPDIR%
GOTO :EOF

:: --------------------
:GET_VERSION_STRING
:: --------------------
set GIT_BUILDTYPE=0
:: Get which branch we are on and whether any outstanding commits in current tree
for /f "tokens=1,2 delims=. " %%A in ('"git status -s -b -uno -- . 2>NUL"') do (
    if [%%A] == [##] (
        set GIT_BRANCH=%%B
        if [%%B] neq [master] if [%%B] neq [main] set GIT_BUILDTYPE=1
    ) else (
        if [%%A%%B] neq [] (
            set GIT_QUALIFIER=.P
            set GIT_BUILDTYPE=2
            goto :gotQualifier
        )
    )
)
:gotQualifier
:: error then no git or not in a repo (ERRORLEVEL not reliable)
IF not defined GIT_BRANCH goto :EOF

if [%GIT_BRANCH%] neq [master] if [%GIT_BRANCH%] neq [main] set GIT_QUALIFIER=%GIT_QUALIFIER%-%GIT_BRANCH%
::
:: get the current SHA1 and commit time for items in this directory
for /f "tokens=1,2" %%A in ('git log -1 "--format=%%h %%ct" -- .') do (
    set GIT_SHA1=%%A
    set UNIX_CTIME=%%B
)
call :gmTime GIT_CTIME UNIX_CTIME

if defined GIT_APPID set strPREFIX=%GIT_APPID%-
:: Use git tag to get the lastest tag applicable to the contents of this directory
for /f "tokens=1"  %%A in ('git tag -l %strPREFIX%[0-9]*.*[0-9] --sort=-v:refname --merged %GIT_SHA1%') do (
    set strTAG=%%A
    goto gotTag
)
:gotTag

if [%strTAG%] neq [] set strFROM=%strTAG%..
:: get the commits in play
:: two options for calculating commits
:: option 1 allows for all commits on all branches
:: option 2 only shows commits for the current branch
:: for /f "tokens=1" %%A in ('git rev-list --branches --count %strFROM%HEAD --until=%UNIX_CTIME% -- .') do set GIT_COMMITS=%%A
for /f "tokens=1" %%A in ('git rev-list --count %strFROM%HEAD -- .') do set GIT_COMMITS=%%A


:: remove any appid prefix or provide default
if [%strTAG%] neq [] (set strTAG=%strTAG:*-=%) else (set strTAG=0.0)
set GIT_VERSION_RC=%strTAG:.=,%,%GIT_COMMITS%,%GIT_BUILDTYPE%
set GIT_VERSION=%strTAG%.%GIT_COMMITS%%GIT_QUALIFIER%
goto :EOF


:: --------------------
:CHECK_CACHE
:: --------------------
:: Exit early if a cached git built version matches the current version.
IF EXIST "%HEADER_OUT_FILE%" (
    IF EXIST "%CACHE_FILE%" (
      if [%fFORCE%] == [1] goto :overwrite
      FOR /F "tokens=* usebackq" %%A IN ("%CACHE_FILE%") DO (
        IF "%%A" == "%GIT_APPID%-%GIT_VERSION%-%GIT_SHA1%" (
          IF NOT DEFINED fQUIET (
            ECHO Build version is assumed unchanged from: %GIT_VERSION%
          )
          SET fDONE=1
        )
      )
    )
)
:overwrite
ECHO %GIT_APPID%-%GIT_VERSION%-%GIT_SHA1%> "%CACHE_FILE%"

GOTO :EOF

:WRITE_OUT
:: -------
IF NOT DEFINED HEADER_OUT_FILE goto :CON_OUT
:: create header file
ECHO // Autogenerated version file>"%HEADER_OUT_FILE%"
set GUARD=v%GIT_VERSION_RC:,=_%
IF DEFINED STDHDR (
ECHO #ifndef %GUARD%>>"%HEADER_OUT_FILE%"
ECHO #define %GUARD%>>"%HEADER_OUT_FILE%"
if [%GIT_APPID%] neq [] ECHO #define GIT_APPID       "%GIT_APPID%">>"%HEADER_OUT_FILE%"
ECHO #define GIT_APPNAME     "%GIT_APPNAME%">>"%HEADER_OUT_FILE%"
ECHO #define GIT_VERSION     "%GIT_VERSION%">>"%HEADER_OUT_FILE%"
ECHO #define GIT_VERSION_RC  %GIT_VERSION_RC% >>"%HEADER_OUT_FILE%"
ECHO #define GIT_SHA1        "%GIT_SHA1%">>"%HEADER_OUT_FILE%"
echo #define GIT_BUILDTYPE   %GIT_BUILDTYPE% >>"%HEADER_OUT_FILE%"
if [%defGIT_APPDIR%] neq [] ECHO #define GIT_APPDIR      "%GIT_APPNAME%">>"%HEADER_OUT_FILE%"
ECHO #define GIT_CTIME       "%GIT_CTIME%">>"%HEADER_OUT_FILE%"
ECHO #define GIT_YEAR        "%GIT_CTIME:~,4%">>"%HEADER_OUT_FILE%"
ECHO #endif>>"%HEADER_OUT_FILE%"
) ELSE (
ECHO namespace GitVersionInfo {>>"%HEADER_OUT_FILE%"
ECHO   public partial class VersionInfo {>>"%HEADER_OUT_FILE%"
ECHO     public const string GIT_APPNAME    = "%GIT_APPNAME%";>>"%HEADER_OUT_FILE%"
ECHO     public const string GIT_VERSION    = "%GIT_VERSION%";>>"%HEADER_OUT_FILE%"
ECHO     public const string GIT_VERSION_RC = "%GIT_VERSION_RC:,=.%";>>"%HEADER_OUT_FILE%"
ECHO     public const string GIT_SHA1       = "%GIT_SHA1%";>>"%HEADER_OUT_FILE%"
ECHO     public const int    GIT_BUILDTYPE  = %GIT_BUILDTYPE%;>>"%HEADER_OUT_FILE%"
ECHO     public const string GIT_CTIME      = "%GIT_CTIME%";>>"%HEADER_OUT_FILE%"
ECHO     public const string GIT_YEAR       = "%GIT_CTIME:~,4%";>>"%HEADER_OUT_FILE%"
ECHO   }>>"%HEADER_OUT_FILE%"
ECHO }>>"%HEADER_OUT_FILE%"
)
:: --------------------
:CON_OUT
:: --------------------
IF DEFINED fQUIET GOTO :EOF
ECHO Git App Id:           %GIT_APPID%
ECHO Git Version:          %GIT_VERSION%
ECHO Build type:           %GIT_BUILDTYPE%
ECHO SHA1:                 %GIT_SHA1%
ECHO App Name              %GIT_APPNAME%
ECHO Committed:            %GIT_CTIME%
GOTO :EOF


:setVar var valpart1 valpar2
    set _var=%1
    set _value=%~2
    if [%3] neq [] set _value=%2 %3
    set _value=%_value:" =%
    set def!_var!=!_value:"=!
goto :eof

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
