@echo off
set UNZIP="guest\7za920\7za.exe"
del gmc4sim-1.39.zip
rmdir /s /q gmc4sim
mkdir gmc4sim
mkdir gmc4sim\doc
copy bin-x86\*.exe gmc4sim
copy bin-x86\*.dll gmc4sim
copy doc\*.pdf gmc4sim\doc
xcopy /s /i sample gmc4sim\sample
%UNZIP% a gmc4sim-1.39.zip gmc4sim
rmdir /s /q gmc4sim
