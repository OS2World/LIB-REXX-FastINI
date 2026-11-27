# Makefile.wat - OpenWatcom 2.0 build for FastINI 98.149
# Run from project root:  wmake -f Makefile.wat
# Full build with log:    compile_wat.cmd
#
# Output:  dll\fastini.dll   32-bit OS/2 REXX DLL

.ERASE

CC    = wcc386
WLINK = wlink

OS2TK = C:\OS2TK45

CFLAGS = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -I=h -I=$(OS2TK)\h

all: directories dll\fastini.dll .SYMBOLIC

directories: .SYMBOLIC
    @if not exist dll md dll
    @if not exist lib md lib

dll\fastini.obj: src\fastini.c h\RexxDll.h src\fastini.def
    $(CC) $(CFLAGS) -fo=$@ src\fastini.c

dll\fastini.dll: dll\fastini.obj dll\fastini.lnk
    $(WLINK) @dll\fastini.lnk

clean: .SYMBOLIC
    @if exist dll\*.obj del dll\*.obj
    @if exist dll\*.map del dll\*.map
    @if exist dll\*.dll del dll\*.dll
