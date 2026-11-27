# LIB-FastINI

FastINI is a REXX DLL that keeps an `.INI` file open while a REXX program
issues `SysIni()` calls, avoiding repeated profile open/close work. The
project includes the DLL, a basic REXX sample and the recommended
`FASTINI.XH` REXX header wrapper.

This tree rebuilds the original library with **Open Watcom 2** for
**ArcaOS** and **OS/2 Warp 4.52**. Public entry-point names, documented
behaviour, copyright/GPL notices and the API-visible version strings are
preserved.

**Current package version: 98.149**

API-visible strings kept unchanged:

- `FastIniVersion`: `98.148`
- `HEADER_FASTINI_XH`: `98.147`

---

## Contents

- [Requirements](#requirements)
- [Quick start](#quick-start)
- [Repository layout](#repository-layout)
- [Build details](#build-details)
- [Verify the build](#verify-the-build)
- [Running the samples](#running-the-samples)
- [Public API](#public-api)
- [Version information](#version-information)
- [Compatibility notes](#compatibility-notes)
- [License and credits](#license-and-credits)

---

## Requirements

Tested on the ArcaOS VM with the toolchain below. The same Toolkit level
targets Warp 4.52.

| Component | Location used by the build |
|---|---|
| Open Watcom 2.0.1 | `C:\WATCOM` |
| OS/2 Toolkit 4.5 headers/libraries | `C:\OS2TK45` |
| OS/2 command processor | `C:\OS2\CMD.EXE` |
| Build tools | `wcc386`, `wmake`, `wlink` |

If Open Watcom or the Toolkit is installed elsewhere, set `WATCOM` and
`OS2TK` before running the scripts, or edit the defaults in
`compile_wat.cmd`.

---

## Quick start

From the project root:

```cmd
compile_wat.cmd
```

That performs a clean build and writes the full compiler/linker output to
`compile.log`.

Expected result:

```text
dll\fastini.dll  rebuilt FastINI REXX DLL
```

---

## Repository layout

| Path | Purpose |
|---|---|
| `Makefile.wat` | Open Watcom 2 `wmake` build file; run from the project root |
| `compile_wat.cmd` | Clean full build; writes `compile.log` |
| `src\fastini.c` | Original C source, with only the trailing EOF marker removed |
| `src\fastini.def` | Original module-definition file, unchanged |
| `dll\fastini.lnk` | Watcom linker directives for the rebuilt DLL |
| `dll\fastini.dll` | Rebuilt 32-bit OS/2 REXX DLL |
| `h\RexxDll.h` | Compatibility header restoring the original support macros |
| `h\FASTINI.XH` | Original recommended REXX header wrapper |
| `sample\EXAMPLE.CMD` | Original basic REXX benchmark sample |
| `sample\FASTINI.X` | Original PPWIZARD source for the header-wrapper sample |
| `doc\` | Original make file, release notes, header notes and GPL notices |

Generated object/map/log files are ignored by Git. The rebuilt DLL is
tracked.

---

## Build details

Manual equivalent of `compile_wat.cmd`:

```cmd
set WATCOM=C:\WATCOM
set OS2TK=C:\OS2TK45
set PATH=%WATCOM%\binp;%WATCOM%\bin;%WATCOM%\bat;%OS2TK%\bin;%PATH%
set INCLUDE=%OS2TK%\h;%WATCOM%\h;%WATCOM%\h\os2
set LIB=%WATCOM%\lib386;%WATCOM%\lib386\os2;%OS2TK%\lib
set DPATH=%OS2TK%\msg
wmake -f Makefile.wat clean
wmake -f Makefile.wat
```

The original project used IBM's C Set++ support makefiles and linked
`REXXDLL.LIB` plus `REXX.LIB`. This rebuild uses:

1. `wcc386 -bt=os2 -mf -5` to compile `src\fastini.c`.
2. `h\RexxDll.h` to supply the missing original support-header macros.
3. `wlink @dll\fastini.lnk` with `clib3r.lib`, `os2386.lib` and `rexx.lib`.

No import library is produced: a REXX DLL is registered by name with
`RxFuncAdd`, and the original package did not ship a C import library.

---

## Verify the build

Check the BLDLEVEL signature:

```cmd
C:\OS2\BLDLEVEL.EXE dll\fastini.dll
```

Expected signature:

```text
@#OS2World:98.149#@FastINI 98.149 (Open Watcom 2)
```

`BLDLEVEL.EXE` displays both revision and file version as `98.149`.

Inspect exports and imports:

```cmd
python lx_export.py dll\fastini.dll --exports --imports
```

The rebuilt DLL exports exactly these three entry points:

```text
1  FastIniStart
2  FastIniVersion
3  FastIniEnd
```

It imports from `REXX`, `PMWIN`, `PMSHAPI`, `NLS` and `DOSCALLS`, matching
the original DLL's dependency set.

---

## Running the samples

Put the DLL directory on `BEGINLIBPATH`, then run either mode:

```cmd
set BEGINLIBPATH=%CD%\dll;%BEGINLIBPATH%
call sample\EXAMPLE.CMD FAST
call sample\EXAMPLE.CMD SLOW
```

Observed on the test VM with 200 `SysIni()` writes:

```text
FAST: about 0.05-0.09 seconds
SLOW: about 6.1-6.2 seconds
```

`FastIniStart()` returns the string `0` on success, not `OK`; the sample's
`Fast Load failed. 0` message is therefore expected and matches the
original DLL's behaviour.

The recommended wrapper sample, `sample\FASTINI.X`, is PPWIZARD source.
Preprocess it with `PPWIZARD ... /rexx` (with `FASTINI.XH` available to the
preprocessor) before running the generated REXX script. An expanded copy
was exercised on the VM; both FAST and SLOW modes completed and printed
the unchanged `98.148` version string.

---

## Public API

| Export | Parameters | Result |
|---|---|---|
| `FastIniVersion` | variable name | Sets that variable to `98.148 http://www.ozemail.com.au/~dbareis db0@anz.com Dennis Bareis` |
| `FastIniStart` | INI filename, handle-variable name | Opens the named INI file and sets the handle variable |
| `FastIniEnd` | handle string | Closes the previously opened INI handle |

These are registered in REXX with:

```rexx
call RxFuncAdd 'FastIniStart',   'FastIni', 'FastIniStart'
call RxFuncAdd 'FastIniEnd',     'FastIni', 'FastIniEnd'
call RxFuncAdd 'FastIniVersion', 'FastIni', 'FastIniVersion'
```

The original `.DEF` did not assign ordinals; the shipped 98.148 DLL exposed
`FastIniStart` as ordinal 1, `FastIniVersion` as 2 and `FastIniEnd` as 3.
The Watcom linker directives preserve those observed ordinals.

---

## Version information

- Package version: **98.149**
- DLL BLDLEVEL signature: `@#OS2World:98.149#@FastINI 98.149 (Open Watcom 2)`
- API version string: `98.148 ...` (unchanged)
- REXX header version: `98.147` (unchanged)

---

## Compatibility notes

- 32-bit OS/2 only. This is not a Win32 or 16-bit build.
- `RexxDll.h` is a reconstruction, not the lost original IBM sample
  header. It reproduces the entry-point signature from
  `os2tk45\h\rexxsaa.h:454-458`, uses `RXSHV_SYSET` from
  `os2tk45\h\rexxsaa.h:143` to set REXX variables through
  `RexxVariablePool` (`os2tk45\h\rexxsaa.h:438`), and keeps the original
  handler convention of returning `0` while reporting status through the
  result `RXSTRING`.
- Error text keeps the original shape:
  `Failure at <source>(<line>): <message>`.
- This is a functional rebuild, not a byte-for-byte reproduction of the
  1998 LINK386 binary.
- No new `C:\POPUPLOG.OS2` faults were recorded while running the DLL and
  both samples on the test VM.

---

## License and credits

Released as Open Source Software under the **GNU GPL v2**. The original
notices are preserved in `COPYING.TXT` and `doc\COPYING.header.txt`.

Original author:

- Dennis Bareis

Original project links:

- <http://dennisbareis.com/>
- <http://www.ozemail.com.au/~dbareis>
