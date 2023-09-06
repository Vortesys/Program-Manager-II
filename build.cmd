@echo off

set DEFS=/DUNICODE
set DIR_ROOT=%~dp0.
set DIR_PROGMGR=%DIR_ROOT%\progmgr
set DIR_RELEASE=%DIR_ROOT%\release

rmdir /S /Q "%DIR_RELEASE%" > nul 2>&1
if not exist "%DIR_RELEASE%" mkdir "%DIR_RELEASE%"

if "%1" == "debug" (
    rem NOTE: This currently doesn't link properly
    set CL=/MT /Od /Zi /RTC1 /Fdprogmgr.pdb /fsanitize=address %DEFS%
    set LINK=/DEBUG msvcrt.lib vcruntime.lib ucrtd.lib
) else (
    set CL=/O1 /DNDEBUG /GS- %DEFS%
    set LINK=/OPT:REF /OPT:ICF
)

pushd "%DIR_RELEASE%"
rc /nologo /i "%DIR_PROGMGR%" "%DIR_PROGMGR%\lang\dlg_en-US.rc"
rc /nologo /i "%DIR_PROGMGR%" "%DIR_PROGMGR%\lang\res_en-US.rc"
rc /nologo /i "%DIR_PROGMGR%" "%DIR_PROGMGR%\resource.rc"
cl /nologo /W3 /WX /MP /Feprogmgr.exe ^
    "%DIR_PROGMGR%\*.c" ^
    "%DIR_PROGMGR%\resource.res" ^
    "%DIR_PROGMGR%\lang\dlg_en-US.res" ^
    "%DIR_PROGMGR%\lang\res_en-US.res" ^
    /link /NODEFAULTLIB /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /FIXED /merge:_RDATA=.rdata ^
    /MANIFEST:EMBED /MANIFESTINPUT:%DIR_PROGMGR%\progmgr.exe.manifest ^
    kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ^
    secur32.lib comdlg32.lib pathcch.lib shlwapi.lib
popd
