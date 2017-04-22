@echo off
set UNZIP="7za920\7za.exe"
rem ---------------------------------------------------------------------------
set VCVERSION=14.0
call "C:\Program Files (x86)\Microsoft Visual Studio %VCVERSION%\VC\vcvarsall.bat"
rem ---------------------------------------------------------------------------
%UNZIP% x -y -owxWidgets-3.1.0 wxWidgets-3.1.0.7z
msbuild wxWidgets-3.1.0\build\msw\wx_vc14.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Release /p:Platform=win32
pause
