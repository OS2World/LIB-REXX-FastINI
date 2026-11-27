@echo off
rem compile_wat.cmd -- Build FastINI with Open Watcom 2.
rem Run from the project root. Writes compile.log in the project root.

if "%WATCOM%" == "" set WATCOM=C:\WATCOM
if "%OS2TK%" == "" set OS2TK=C:\OS2TK45
set PATH=%WATCOM%\binp;%WATCOM%\bin;%WATCOM%\bat;%OS2TK%\bin;%PATH%
set INCLUDE=%OS2TK%\h;%WATCOM%\h;%WATCOM%\h\os2
set LIB=%WATCOM%\lib386;%WATCOM%\lib386\os2;%OS2TK%\lib;%LIB%
set DPATH=%OS2TK%\msg;%DPATH%

echo.
echo === FastINI 98.149 -- Open Watcom 32-bit OS/2 build ===
echo.

wmake -f Makefile.wat clean < NUL
wmake -f Makefile.wat < NUL > compile.log 2>&1
if errorlevel 1 goto buildfail

type compile.log
echo.
echo === Build complete ===
echo   dll\fastini.dll  -- rebuilt FastINI REXX DLL
goto buildend

:buildfail
type compile.log
echo.
echo *** Build FAILED -- see compile.log ***

:buildend
