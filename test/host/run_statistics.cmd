@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cl /nologo /EHsc /std:c++14 /Itest\host /Isrc\Core test\host\statistics.cpp src\Core\System.cpp src\Core\Hardware\SimulationBackend.cpp src\Core\Histogram.cpp src\Core\Buffer.cpp /Fotmp\range-tests\ /Fetmp\range-tests\statistics.exe
if errorlevel 1 exit /b 1
tmp\range-tests\statistics.exe
