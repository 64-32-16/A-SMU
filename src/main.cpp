#include <Arduino.h>

#include "Core/Beeper.h"
#include "Core/System.h"
#include "drivers/Ltc2602.h"
#include "UI/Core/Container.h"
#include "UI/Core/GDClass.h"
#include "UI/Core/Theme.h"
#include "UI/Pages/MainPage.h"
#include "UI/Pages/TracePage.h"
#include "UI/Panels/MeasureModePopup.h"
#include "UI/Panels/NumberPadPopup.h"
#include "UI/Panels/OutputOffStatePopup.h"
#include "UI/Panels/ResistanceRangePopup.h"
#include "UI/Panels/SourcePanel.h"
#include "UI/Panels/SourceModePopup.h"
#include "UI/Panels/SourceRangePopup.h"
#include "UI/Panels/XYGraphPopup.h"

constexpr uint32_t FRAME_TIME_MS = 20;
constexpr uint8_t BEEPER_PIN = 9;

Container root(0, 0, 800, 480);
MainPage mainPage(0, 0, 800, 480);
TracePage tracePage(0, 0, 800, 480);
SourceRangePopup sourceRangePopup(0, 0, 800, 480);
SourceModePopup sourceModePopup(0, 0, 800, 480);
ResistanceRangePopup resistanceRangePopup(0, 0, 800, 480);
NumberPadPopup numberPadPopup(0, 0, 800, 480);
MeasureModePopup measureModePopup(0, 0, 800, 480);
OutputOffStatePopup outputOffStatePopup(0, 0, 800, 480);
XYGraphPopup xyGraphPopup(0, 0, 800, 480);
Ltc2602 prototypeDac;

uint32_t lastFrameMs = 0;
bool lastTouching = false;
int16_t lastTouchX = 0;
int16_t lastTouchY = 0;
bool sourceExpanded = false;

void ApplyLayout()
{
    mainPage.SetBounds(0, 0, 800, 480);
    mainPage.UpdateLayout();
    tracePage.SetBounds(0, 0, 800, 480);
    tracePage.UpdateLayout();
}

void UpdateCaptionFromMode()
{
    mainPage.GetMeasurePanel().SetCaption(nullptr);
}

void ShowMainPage()
{
    mainPage.SetVisible(true);
    tracePage.SetVisible(false);
}

void ShowTracePage()
{
    mainPage.SetVisible(false);
    tracePage.SetVisible(true);
    tracePage.SetActiveView(TraceConfigView::Graph);
}

void OpenMainPage(Button* sender)
{
    if (sender != &mainPage.GetHeaderPanel().GetHomeButton() &&
        sender != &tracePage.GetHeaderPanel().GetHomeButton())
    {
        return;
    }

    ShowMainPage();
}

void OpenTracePage(Button* sender)
{
    if (sender != &mainPage.GetConfigPanel().GetTraceButton())
    {
        return;
    }

    ShowTracePage();
}

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

void OnSourceExpandChanged(SourcePanel* sender, bool expanded)
{
    if (sender != &mainPage.GetSourcePanel())
    {
        return;
    }

    sourceExpanded = expanded;
    ApplyLayout();
}

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
    while (!Serial) {}
    delay(200);

    Serial.println("A-SMU Measure + SourcePanel test start");

    GD.begin(0);
    GD.wr(REG_PWM_DUTY, 50);

    Beeper.Begin(BEEPER_PIN, true);

    System.Reset();
    System.SetSimulationEnabled(true);
    System.SetMeasureMode(MeasureMode::Voltage);
    System.SetSenseMode(SenseMode::Local);
    System.SetMeasureAutoUnit(true);
    System.SetVoltage(1.2345f);
    System.SetCurrent(0.0123f);
    System.SetResistance(1234.0f);
    System.SetPower(0.0152f);
    System.SetVoltageSourceValue(0.0f);
    System.SetVoltageLimitValue(0.0f);
    System.SetCurrentSourceValue(0.0f);
    System.SetCurrentLimitValue(0.0f);
    System.SetSourceMode(SourceMode::Voltage);
    System.SetDacOutput(&prototypeDac);

    mainPage.SetSystem(&System);
    tracePage.SetSystem(&System);
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
    tracePage.GetHeaderPanel().GetHomeButton().SetOnClick(OpenMainPage);
    mainPage.GetConfigPanel().GetTraceButton().SetOnClick(OpenTracePage);
    sourcePanel.SetOnExpandChanged(OnSourceExpandChanged);
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
    root.AddChild(&tracePage);

    root.AddChild(&sourceRangePopup);
    root.AddChild(&sourceModePopup);
    root.AddChild(&numberPadPopup);
    root.AddChild(&outputOffStatePopup);
    root.AddChild(&xyGraphPopup);

    root.AddChild(&resistanceRangePopup);
    root.AddChild(&measureModePopup);

    Serial.println("Setup done");
}

void loop()
{
    Beeper.Update();
    System.Execute();
    UpdateCaptionFromMode();

    const uint32_t now = millis();

    if ((now - lastFrameMs) < FRAME_TIME_MS)
    {
        return;
    }

    lastFrameMs = now;

    GD.get_inputs();

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

    GD.cmd_dlstart();
    GD.cmd_romfont(1, 34);
    GD.cmd_romfont(2, 33);
    GD.ClearColorRGB(Theme::MainScreenBackground);
    GD.Clear();
    GD.VertexFormat(0);

    GD.ColorRGB(Theme::PrototypeFrame);
    GD.LineWidth(16);
    GD.Begin(LINE_STRIP);
    GD.Vertex2f(1, 1);
    GD.Vertex2f(799, 1);
    GD.Vertex2f(799, 479);
    GD.Vertex2f(1, 479);
    GD.Vertex2f(1, 1);

    root.Draw();

    GD.swap();
}
