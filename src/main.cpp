#include <Arduino.h>

#include "Core/Beeper.h"
#include "Core/System.h"
#include "Core/Hardware/SimulationAcquisition.h"
#include "UI/Core/Container.h"
#include "UI/Core/GDClass.h"
#include "UI/Core/Theme.h"
#include "UI/Pages/MainPage.h"
#include "UI/Pages/MenuPage.h"
#include "UI/Pages/InfoPage.h"
#include "UI/Pages/MeasureSettingsPage.h"
#include "UI/Pages/SourceSettingsPage.h"
#include "UI/Pages/SystemSettingsPage.h"
#include "UI/Core/WakeTouchGate.h"
#include "UI/Core/UiTiming.h"
#include "UI/Pages/SimulationPage.h"
#include "UI/Pages/GraphPage.h"
#include "UI/Pages/HistogramPage.h"
#include "UI/Pages/ReadingTablePage.h"
#include "UI/Panels/MeasureModePopup.h"
#include "UI/Panels/NumberPadPopup.h"
#include "UI/Panels/OutputOffStatePopup.h"
#include "UI/Panels/ResistanceRangePopup.h"
#include "UI/Panels/SourcePanel.h"
#include "UI/Panels/SourceModePopup.h"
#include "UI/Panels/SourceRangePopup.h"
#include "UI/Panels/XYGraphPopup.h"

constexpr uint32_t FRAME_TIME_MS = 33;
constexpr uint32_t TOUCH_TIME_MS = 10;
Hardware::SimulationAcquisition acquisition;
// Preserve Core 0's 8 KiB stack; Arduino-Pico allocates another 8 KiB for Core 1.
bool core1_separate_stack = true;

void setup1() {acquisition.WorkerReady();}
void loop1() {
    acquisition.RunOnce();
    // No Arduino delay/USB/EVE on Core 1. Bound simulation polling latency.
    busy_wait_us_32(50);
}
constexpr uint32_t SERIAL_WAIT_TIMEOUT_MS = 1000;
constexpr uint8_t BEEPER_PIN = 9;

Container root(0, 0, 800, 480);
MainPage mainPage(0, 0, 800, 480);
MenuPage menuPage(0, 0, 800, 480);
InfoPage infoPage(0,0,800,480);
SourceSettingsPage sourceSettingsPage(0,0,800,480);
MeasureSettingsPage measureSettingsPage(0,0,800,480);
SystemSettingsPage systemSettingsPage(0,0,800,480);
WakeTouchGate wakeTouch;
SimulationPage simulationPage(0,0,800,480);
GraphPage graphPage(0, 0, 800, 480);
HistogramPage histogramPage(0,0,800,480);
ReadingTablePage readingTablePage(0,0,800,480);
SourceRangePopup sourceRangePopup(0, 0, 800, 480);
SourceModePopup sourceModePopup(0, 0, 800, 480);
ResistanceRangePopup resistanceRangePopup(0, 0, 800, 480);
NumberPadPopup numberPadPopup(0, 0, 800, 480);
MeasureModePopup measureModePopup(0, 0, 800, 480);
OutputOffStatePopup outputOffStatePopup(0, 0, 800, 480);
XYGraphPopup xyGraphPopup(0, 0, 800, 480);

uint32_t lastFrameMs = 0;
uint32_t lastTouchMs = 0;
bool lastTouching = false;
int16_t lastTouchX = 0;
int16_t lastTouchY = 0;

void ApplyLayout()
{
    mainPage.SetBounds(0, 0, 800, 480);
    mainPage.UpdateLayout();
}

void UpdateCaptionFromMode()
{
    mainPage.GetMeasurePanel().SetCaption(nullptr);
}

void ShowMainPage()
{
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    systemSettingsPage.SetVisible(false);
    measureSettingsPage.SetVisible(false);
    simulationPage.SetVisible(false);
    readingTablePage.SetVisible(false);
    histogramPage.SetVisible(false);
    graphPage.SetVisible(false);
    menuPage.SetVisible(false);
    mainPage.SetVisible(true);
    mainPage.GetSourcePanel().ShowSource();
}

void OpenMainPage(Button* sender)
{
    (void)sender;
    ShowMainPage();
}

void OpenMenu(Button* sender)
{
    if (sender == &mainPage.GetHeaderPanel().GetMenuButton())
    {
        mainPage.SetVisible(false);
        menuPage.Open();
    }
}

void OpenGraphPage(Button* sender)
{
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    systemSettingsPage.SetVisible(false);
    measureSettingsPage.SetVisible(false);
    simulationPage.SetVisible(false);
    readingTablePage.SetVisible(false);
    (void)sender;
    histogramPage.SetVisible(false);mainPage.SetVisible(false);menuPage.SetVisible(false);graphPage.Open();
}

void BackFromGraph(Button* sender)
{
    (void)sender;
    graphPage.SetVisible(false);menuPage.Open();
}

void OpenHistogramPage(Button* sender)
{
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    systemSettingsPage.SetVisible(false);
    measureSettingsPage.SetVisible(false);
    simulationPage.SetVisible(false);
    readingTablePage.SetVisible(false);
    (void)sender;mainPage.SetVisible(false);graphPage.SetVisible(false);menuPage.SetVisible(false);histogramPage.Open();
}
void BackFromHistogram(Button* sender)
{
    (void)sender;histogramPage.SetVisible(false);menuPage.Open();
}

void OpenReadingTablePage(Button* sender)
{
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    systemSettingsPage.SetVisible(false);
    measureSettingsPage.SetVisible(false);
    simulationPage.SetVisible(false);
    (void)sender;mainPage.SetVisible(false);graphPage.SetVisible(false);histogramPage.SetVisible(false);
    menuPage.SetVisible(false);readingTablePage.Open();
}
void BackFromReadingTable(Button* sender)
{
    (void)sender;readingTablePage.SetVisible(false);menuPage.Open();
}

void OpenSourceSettingsPage(Button*) {
    ShowMainPage();mainPage.SetVisible(false);sourceSettingsPage.Open();
}
void BackFromSourceSettings(Button*) {sourceSettingsPage.SetVisible(false);menuPage.Open();}
void EditSourceSetting(SourceSettingsPage::Setting setting) {
    switch(setting) {
        case SourceSettingsPage::Setting::Function: sourceModePopup.Open();break;
        case SourceSettingsPage::Setting::Range: sourceRangePopup.OpenForMode(System.GetSourceMode());break;
        case SourceSettingsPage::Setting::Limit: numberPadPopup.Open(System.GetSourceMode(),NumberPadTargetType::LimitValue);break;
        case SourceSettingsPage::Setting::OutputOff: outputOffStatePopup.Open();break;
    }
}
void OpenInfoPage(Button*) {
    ShowMainPage();mainPage.SetVisible(false);infoPage.Open();
}
void BackFromInfo(Button*) {infoPage.SetVisible(false);menuPage.Open();}
void OpenSystemSettingsPage(Button*) {
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    mainPage.SetVisible(false);menuPage.SetVisible(false);graphPage.SetVisible(false);
    histogramPage.SetVisible(false);readingTablePage.SetVisible(false);simulationPage.SetVisible(false);
    measureSettingsPage.SetVisible(false);systemSettingsPage.Open();
}
void BackFromSystemSettings(Button*) {systemSettingsPage.SetVisible(false);menuPage.Open();}
void OpenMeasureSettingsPage(Button*) {
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    systemSettingsPage.SetVisible(false);
    mainPage.SetVisible(false);menuPage.SetVisible(false);graphPage.SetVisible(false);
    histogramPage.SetVisible(false);readingTablePage.SetVisible(false);simulationPage.SetVisible(false);
    measureSettingsPage.Open();
}
void BackFromMeasureSettings(Button*) {measureSettingsPage.SetVisible(false);menuPage.Open();}
void OpenSimulationPage(Button* sender) {
    sourceSettingsPage.SetVisible(false);
    infoPage.SetVisible(false);
    systemSettingsPage.SetVisible(false);
    measureSettingsPage.SetVisible(false);
    (void)sender;mainPage.SetVisible(false);menuPage.SetVisible(false);graphPage.SetVisible(false);
    histogramPage.SetVisible(false);readingTablePage.SetVisible(false);simulationPage.Open();
}
void BackFromSimulation(Button* sender) {(void)sender;simulationPage.SetVisible(false);menuPage.Open();}

void OpenMeasureModePopup(Button* sender)
{
    if (sender != &mainPage.GetMeasurePanel().GetFunctionButton())
    {
        return;
    }

    measureModePopup.Open();
}

void OnMeasureModePopupClosed(MeasureModePopup* sender, bool accepted)
{
    if (sender != &measureModePopup)
    {
        return;
    }

    if (accepted)
    {
        UpdateCaptionFromMode();
    }
}

void OpenMeasureSettingsRange(SourceMode mode) {sourceRangePopup.OpenForMode(mode);}

void OpenSourceRangePopup(Button* sender)
{
    if (sender == &mainPage.GetMeasurePanel().GetRangeButton())
    {
        if (System.GetMeasureMode() == MeasureMode::Voltage)
        {
            sourceRangePopup.OpenForMode(SourceMode::Voltage);
        }
        else if (System.GetMeasureMode() == MeasureMode::Current)
        {
            sourceRangePopup.OpenForMode(SourceMode::Current);
        }
        else if (System.GetMeasureMode() == MeasureMode::Resistance)
        {
            resistanceRangePopup.Open();
        }
        return;
    }

    const SourceMode mode = System.GetSourceMode();
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();
    const bool isVoltageSender = (sender == &sourcePanel.GetVoltagePanel().GetRangeButton());
    const bool isCurrentSender = (sender == &sourcePanel.GetCurrentPanel().GetRangeButton());

    if ((mode == SourceMode::Voltage && isVoltageSender) ||
        (mode == SourceMode::Current && isCurrentSender))
    {
        sourceRangePopup.OpenForMode(mode);
    }
}

void OpenNumberPadPopup(Button* sender)
{
    const SourceMode mode = System.GetSourceMode();
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();

    if (mode == SourceMode::Voltage)
    {
        if (sender == &sourcePanel.GetVoltagePanel().GetSourceButton())
        {
            numberPadPopup.Open(mode, NumberPadTargetType::SourceValue);
        }
        else if (sender == &sourcePanel.GetVoltagePanel().GetLimitButton())
        {
            numberPadPopup.Open(mode, NumberPadTargetType::LimitValue);
        }
        return;
    }

    if (sender == &sourcePanel.GetCurrentPanel().GetSourceButton())
    {
        numberPadPopup.Open(mode, NumberPadTargetType::SourceValue);
    }
    else if (sender == &sourcePanel.GetCurrentPanel().GetLimitButton())
    {
        numberPadPopup.Open(mode, NumberPadTargetType::LimitValue);
    }
}

void OpenSourceModePopup(Button* sender)
{
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();
    if (sender != &sourcePanel.GetVoltagePanel().GetSourceModeButton() &&
        sender != &sourcePanel.GetCurrentPanel().GetSourceModeButton())
    {
        return;
    }

    sourceModePopup.Open();
}

void OpenOutputOffStatePopup(Button* sender)
{
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();
    if (sender != &sourcePanel.GetMenuPanel().GetOutputOffStateButton())
    {
        return;
    }

    outputOffStatePopup.Open();
}

void OpenXYGraphPopup(Button* sender)
{
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();
    if (sender != &sourcePanel.GetMenuPanel().GetGraphButton())
    {
        return;
    }

    xyGraphPopup.Open();
}

void setup()
{
    Serial.begin(115200);
    const uint32_t serialWaitStartMs = millis();
    while (!Serial && (millis() - serialWaitStartMs) < SERIAL_WAIT_TIMEOUT_MS) {}
    delay(200);

    Serial.println("A-SMU Arduino-Pico / UI Core 0 / simulation Core 1");

    GD.begin(0);
    GD.SetBacklightPercent(System.GetEffectiveBacklightPercent());

    Beeper.Begin(BEEPER_PIN, true);
    Beeper.SetClickAllowed([](){return System.GetUserInterfaceSettings().keyClick;});

    System.Reset();
    System.SetSimulationEnabled(true);
    // GUI development currently assumes an always-enabled simulated output.
    System.SetOutputOn(true);
    System.SetMeasureMode(MeasureMode::Voltage);
    System.SetSenseMode(SenseMode::Local);
    System.SetMeasureAutoUnit(true);
    System.SetVoltageSourceValue(0.0f);
    System.SetVoltageLimitValue(0.0f);
    System.SetCurrentSourceValue(0.0f);
    System.SetCurrentLimitValue(0.0f);
    System.SetSourceMode(SourceMode::Voltage);

    mainPage.SetSystem(&System);
    sourceRangePopup.SetSystem(&System);
    sourceModePopup.SetSystem(&System);
    resistanceRangePopup.SetSystem(&System);
    numberPadPopup.SetSystem(&System);
    measureModePopup.SetSystem(&System);
    outputOffStatePopup.SetSystem(&System);
    xyGraphPopup.SetSystem(&System);
    measureModePopup.SetOnClose(OnMeasureModePopupClosed);
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();
    mainPage.GetHeaderPanel().GetHomeButton().SetOnClick(OpenMainPage);
    mainPage.GetHeaderPanel().GetMenuButton().SetOnClick(OpenMenu);
    menuPage.SetOnHome(OpenMainPage);
    menuPage.SetOnSourceSettings(OpenSourceSettingsPage);
    sourceSettingsPage.SetSystem(&System);
    sourceSettingsPage.SetOnBack(BackFromSourceSettings);
    sourceSettingsPage.SetOnHome(OpenMainPage);
    sourceSettingsPage.SetOnSetting(EditSourceSetting);
    menuPage.SetOnInfo(OpenInfoPage);
    infoPage.SetOnBack(BackFromInfo);
    infoPage.SetOnHome(OpenMainPage);
    menuPage.SetOnSimulation(OpenSimulationPage);
    menuPage.SetOnSystemSettings(OpenSystemSettingsPage);
    systemSettingsPage.SetSystem(&System);
    systemSettingsPage.SetOnBack(BackFromSystemSettings);
    systemSettingsPage.SetOnHome(OpenMainPage);
    menuPage.SetOnMeasureSettings(OpenMeasureSettingsPage);
    measureSettingsPage.SetSystem(&System);
    measureSettingsPage.SetOnRange(OpenMeasureSettingsRange);
    measureSettingsPage.SetOnBack(BackFromMeasureSettings);
    measureSettingsPage.SetOnHome(OpenMainPage);
    simulationPage.SetSystem(&System);
    simulationPage.SetOnBack(BackFromSimulation);
    simulationPage.SetOnHome(OpenMainPage);
    menuPage.SetOnGraph(OpenGraphPage);
    menuPage.SetOnHistogram(OpenHistogramPage);
    menuPage.SetOnReadingTable(OpenReadingTablePage);
    readingTablePage.SetSystem(&System);
    readingTablePage.SetOnBack(BackFromReadingTable);
    readingTablePage.SetOnHome(OpenMainPage);
    histogramPage.SetSystem(&System);
    histogramPage.SetOnBack(BackFromHistogram);
    histogramPage.SetOnHome(OpenMainPage);
    graphPage.SetSystem(&System);
    graphPage.SetOnBack(BackFromGraph);
    graphPage.SetOnHome(OpenMainPage);
    mainPage.GetMeasurePanel().GetRangeButton().SetOnClick(OpenSourceRangePopup);
    mainPage.GetMeasurePanel().GetFunctionButton().SetOnClick(OpenMeasureModePopup);
    sourcePanel.GetVoltagePanel().GetRangeButton().SetOnClick(OpenSourceRangePopup);
    sourcePanel.GetCurrentPanel().GetRangeButton().SetOnClick(OpenSourceRangePopup);
    sourcePanel.GetVoltagePanel().GetSourceModeButton().SetOnClick(OpenSourceModePopup);
    sourcePanel.GetCurrentPanel().GetSourceModeButton().SetOnClick(OpenSourceModePopup);
    sourcePanel.GetVoltagePanel().GetSourceButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetVoltagePanel().GetLimitButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetCurrentPanel().GetSourceButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetCurrentPanel().GetLimitButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetMenuPanel().GetOutputOffStateButton().SetOnClick(OpenOutputOffStatePopup);
    sourcePanel.GetMenuPanel().GetGraphButton().SetOnClick(OpenXYGraphPopup);
    UpdateCaptionFromMode();
    ShowMainPage();
    ApplyLayout();

    root.AddChild(&mainPage);
    root.AddChild(&menuPage);
    root.AddChild(&infoPage);
    root.AddChild(&sourceSettingsPage);
    root.AddChild(&measureSettingsPage);
    root.AddChild(&systemSettingsPage);
    root.AddChild(&graphPage);
    root.AddChild(&histogramPage);
    root.AddChild(&readingTablePage);
    root.AddChild(&simulationPage);

    root.AddChild(&sourceRangePopup);
    root.AddChild(&sourceModePopup);
    root.AddChild(&numberPadPopup);
    root.AddChild(&outputOffStatePopup);
    root.AddChild(&xyGraphPopup);

    root.AddChild(&resistanceRangePopup);
    root.AddChild(&measureModePopup);

    const uint32_t workerWaitStart=millis();
    bool workerStarted=false;
    do {
        workerStarted=acquisition.Start();
        if(!workerStarted) delay(1);
    } while(!workerStarted && millis()-workerWaitStart<500);
    if(workerStarted) {
        System.SetAcquisition(&acquisition);
        Serial.println("Core 1 acquisition ready");
    } else Serial.println("ERROR: Core 1 not ready; synchronous simulation fallback");
    Serial.println("Setup done");
}

void loop()
{
    UI_TIMING_REPORT();
    UI_TIMED(System, System.Execute());
    UpdateCaptionFromMode();

    const uint32_t now = millis();
    System.UpdateUserInterface(now);

    if ((now - lastTouchMs) >= TOUCH_TIME_MS)
    {
        lastTouchMs = now;
        UI_TOUCH_POLL();
        UI_TIMED(Input, GD.get_inputs());
        UI_TIMED(Dispatch, {
        const bool anyTouch=GD.inputs.touching || GD.touches.count;
        const bool consumeWake=wakeTouch.Consume(anyTouch,System.IsBacklightDimmed());
        if(anyTouch) System.RegisterUserActivity(now);
        if(consumeWake) {
            if(lastTouching) root.TouchEnd(-1,-1);
            lastTouching=false;
        } else {

            // Only the topmost visible surface receives new gestures (never behind a modal).
            Control* touchSurface=nullptr;
            for(int i=root.GetChildCount()-1;i>=0;--i)
            {
                Control* child=root.GetChild(i);
                if(child->IsVisible() && child->IsEnabled()) {touchSurface=child;break;}
            }
            if(readingTablePage.HandleMultiTouch(GD.touches,touchSurface==&readingTablePage) ||
               histogramPage.HandleMultiTouch(GD.touches,touchSurface==&histogramPage) ||
               graphPage.HandleMultiTouch(GD.touches,touchSurface==&graphPage) ||
               mainPage.GetSourcePanel().HandleMultiTouch(GD.touches,touchSurface==&mainPage))
            {
                if(lastTouching) root.TouchEnd(-1,-1);
                lastTouching=false;
            }
            else
            {
                const bool touching = GD.inputs.touching;

                if (touching)
                {
                    lastTouchX = GD.inputs.x;
                    lastTouchY = GD.inputs.y;
                }

                if (touching && !lastTouching)
                {
                    root.TouchStart(lastTouchX, lastTouchY);
                }

                if (touching)
                {
                    root.TouchMove(lastTouchX, lastTouchY);
                }

                if (!touching && lastTouching)
                {
                    root.TouchEnd(lastTouchX, lastTouchY);
                }

                lastTouching = touching;

            }

        } // wake gestures never reach buttons or multi-touch handlers
        });
    } // independent touch cadence, also while waiting for a new measurement

    // Publish control changes before drawing; keep the history stable for the frame.
    UI_TIMED(System, System.Execute());
    GD.SetBacklightPercent(System.GetEffectiveBacklightPercent());
    if ((now - lastFrameMs) < FRAME_TIME_MS) return;
    bool frameReady;
    UI_TIMED(Begin, frameReady = GD.BeginFrame());
    if (!frameReady) return;
    lastFrameMs=now;
    GD.cmd_romfont(1, 34);
    GD.cmd_romfont(2, 33);
    GD.ClearColorRGB(Theme::MainScreenBackground);
    GD.Clear();
    GD.VertexFormat(0);

    GD.ColorRGB(Theme::ScreenFrame);
    GD.LineWidth(16);
    GD.Begin(LINE_STRIP);
    GD.Vertex2f(1, 1);
    GD.Vertex2f(799, 1);
    GD.Vertex2f(799, 479);
    GD.Vertex2f(1, 479);
    GD.Vertex2f(1, 1);

    Control* vectorSurface=mainPage.IsVisible()?static_cast<Control*>(&mainPage):
        graphPage.IsVisible()?static_cast<Control*>(&graphPage):nullptr;
    bool allowVectors=vectorSurface!=nullptr;
    for(int i=0;i<root.GetChildCount();++i) {
        Control* child=root.GetChild(i);
        if(child!=vectorSurface && child->IsVisible()) allowVectors=false;
    }
    // MENU draws controls first; reserve room for remaining bitmap curves,
    // cursors and DISPLAY. Main also needs navigation and MeasurePanel.
    GD.AllowPlotVectors(allowVectors,vectorSurface==&graphPage?1024:2048);
    UI_TIMED(Draw, root.Draw());

    UI_TIMED(Swap, GD.swap());

}
