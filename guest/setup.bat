@echo off
set UNZIP="7za920\7za.exe"
rem ---------------------------------------------------------------------------
set VCVERSION=14.0
call "C:\Program Files (x86)\Microsoft Visual Studio %VCVERSION%\VC\vcvarsall.bat"
rem ---------------------------------------------------------------------------
%UNZIP% x -y wxMSW-2.8.12.zip
%UNZIP% x -y wxMSW-2.8.12-gmc4sim-patch.zip
msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Release /p:Platform=win32
rem msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Release /p:Platform=x64
rem msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Debug /p:Platform=win32
rem msbuild wxMSW-2.8.12\build\msw\wx.sln /clp:DisableConsoleColor /t:Build /p:Configuration=Debug /p:Platform=x64
pause
