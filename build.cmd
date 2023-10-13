@echo off

set DIR_ROOT=%~dp0.
set DIR_PROGMGR=%DIR_ROOT%\progmgr
set DIR_RELEASE=%DIR_ROOT%\release

rmdir /S /Q "%DIR_RELEASE%" > nul 2>&1
if not exist "%DIR_RELEASE%" mkdir "%DIR_RELEASE%"

git rev-parse HEAD > %DIR_RELEASE%\.git_hash
set /P GIT_HASH=<%DIR_RELEASE%\.git_hash
set "GIT_HASH=%GIT_HASH:~0,8%"

if "%1" == "debug" (
    set CL=/MT /Od /Zi /RTC1 /Fdprogmgr.pdb /fsanitize=address /DUNICODE /D_DEBUG /DPROGMGR_DEBUG /DPROGMGR_GIT_HASH="%GIT_HASH%"
    set LINK=/DEBUG
) else (
    set CL=/O1 /GS- /DUNICODE /DNDEBUG /DPROGMGR_RELEASE /DPROGMGR_GIT_HASH="%GIT_HASH%"
    rem set LINK=/NODEFAULTLIB /OPT:REF /OPT:ICF
    set LINK=/OPT:REF /OPT:ICF
)

pushd "%DIR_RELEASE%"
rc /nologo /DPROGMGR_GIT_HASH="""%GIT_HASH%""" /i "%DIR_PROGMGR%" "%DIR_PROGMGR%\lang\dlg_en-US.rc"
rc /nologo /DPROGMGR_GIT_HASH="""%GIT_HASH%""" /i "%DIR_PROGMGR%" "%DIR_PROGMGR%\lang\res_en-US.rc"
rc /nologo /DPROGMGR_GIT_HASH="""%GIT_HASH%""" /i "%DIR_PROGMGR%" "%DIR_PROGMGR%\resource.rc"
cl /nologo /W3 /WX /MP /Feprogmgr.exe ^
    "%DIR_PROGMGR%\*.c" ^
    "%DIR_PROGMGR%\resource.res" ^
    "%DIR_PROGMGR%\lang\dlg_en-US.res" ^
    "%DIR_PROGMGR%\lang\res_en-US.res" ^
    /link /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /FIXED /merge:_RDATA=.rdata ^
    /MANIFEST:EMBED /MANIFESTINPUT:%DIR_PROGMGR%\progmgr.exe.manifest ^
    kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ^
    secur32.lib comdlg32.lib pathcch.lib shlwapi.lib version.lib ^
    comctl32.lib uxtheme.lib
popd
