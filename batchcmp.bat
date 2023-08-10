7z a -r "%~dp0bin\bin.7z" "%~dp0bin\*.exe"
WinRAR a -r "%~dp0bin\bin.rar" "%~dp0bin\*.exe"
del /s "%~dp0bin\*.exe"
pause