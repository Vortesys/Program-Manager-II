del /s "%~dp0\*.7z"
del /s "%~dp0\*.rar"
7z a -r "%~dp0\bin.7z" "%~dp0bin\*.exe"
WinRAR a -r "%~dp0\bin.rar" "%~dp0bin\*.exe"
del /s "%~dp0bin\*.exe"
pause