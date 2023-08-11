@echo on

msbuild Program-Manager-II.sln -target:progmgr

del /s "%~dp0\*.7z"
del /s "%~dp0\*.rar"
del /s "%~dp0bin\*.exe"
7z a -r "%~dp0\bin.7z" "%~dp0\bin\*.exe"
WinRAR a -r "%~dp0\bin.rar" "%~dp0\bin\*.exe"
pause