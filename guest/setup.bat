@echo off
set UNZIP="C:\Program Files\7-Zip\7z.exe"
if not exist %UNZIP% set UNZIP="C:\Program Files (x86)\7-Zip\7z.exe"
rem ---------------------------------------------------------------------------
set VCVARSALL="C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
if not exist %VCVARSALL% set VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
call %VCVARSALL%
rem ---------------------------------------------------------------------------
%UNZIP% x -y wxMSW-2.8.12.zip
%UNZIP% x -y wxMSW-2.8.12-gmc4sim-patch.zip
msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Release /p:Platform=win32
rem msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Release /p:Platform=x64
rem msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Debug /p:Platform=win32
rem msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Debug /p:Platform=x64
pause
