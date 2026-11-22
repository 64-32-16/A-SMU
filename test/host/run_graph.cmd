@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cl /nologo /EHsc /std:c++14 /D_CRT_SECURE_NO_WARNINGS /Itest\host /Isrc test\host\graph.cpp src\Core\System.cpp src\Core\Hardware\SimulationBackend.cpp src\Core\Histogram.cpp src\Core\Buffer.cpp src\UI\Core\GraphState.cpp src\UI\Core\DisplayFormat.cpp src\UI\Core\Control.cpp src\UI\Core\Container.cpp src\UI\Core\Panel.cpp src\UI\Controls\Button.cpp src\UI\Controls\TabControl.cpp src\UI\Pages\MenuPage.cpp src\UI\Pages\GraphPage.cpp src\UI\Pages\HistogramPage.cpp src\UI\Pages\ReadingTablePage.cpp src\UI\Pages\SimulationPage.cpp src\UI\Core\ReadingTableState.cpp src\UI\Panels\NumericEntryDialog.cpp src\UI\Panels\GraphPanel.cpp src\UI\Panels\SourcePanel.cpp src\UI\Panels\VoltageSourcePanel.cpp src\UI\Panels\CurrentSourcePanel.cpp src\UI\Panels\StatisticsPanel.cpp src\UI\Panels\MenuPanel.cpp src\UI\Controls\Label.cpp src\UI\Controls\Divider.cpp /Fotmp\range-tests\ /Fetmp\range-tests\graph.exe
if errorlevel 1 exit /b 1
tmp\range-tests\graph.exe
