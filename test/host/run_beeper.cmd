@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cl /nologo /EHsc /std:c++14 /Itest\host /Isrc test\host\beeper_timing.cpp src\Core\Beeper.cpp /Fotmp\range-tests\ /Fetmp\range-tests\beeper.exe
if errorlevel 1 exit /b 1
tmp\range-tests\beeper.exe
