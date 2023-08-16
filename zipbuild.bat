del /s "%~dp0\*.zip"
7z a -r "%~dp0\bin.zip" "%~dp0bin\*.exe"
pause