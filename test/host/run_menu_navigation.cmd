@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cl /nologo /EHsc /std:c++14 /D_CRT_SECURE_NO_WARNINGS /Itest\host /Isrc test\host\menu_navigation.cpp src\UI\Core\Control.cpp src\UI\Core\Container.cpp src\UI\Core\Panel.cpp src\UI\Controls\Button.cpp src\UI\Pages\MenuPage.cpp src\Core\System.cpp src\Core\Hardware\SimulationBackend.cpp src\Core\Histogram.cpp src\Core\Buffer.cpp /Fotmp\range-tests\ /Fetmp\range-tests\menu_navigation.exe
if errorlevel 1 exit /b 1
tmp\range-tests\menu_navigation.exe
