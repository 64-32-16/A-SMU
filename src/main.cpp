#include <Arduino.h>

#include "Core/Beeper.h"
#include "Core/System.h"
#include "drivers/Ltc2602.h"
#include "UI/Core/Container.h"
#include "UI/Core/GDClass.h"
#include "UI/Core/Theme.h"
#include "UI/Pages/MainPage.h"
#include "UI/Panels/MeasureFunctionPopup.h"
#include "UI/Panels/MeasurePanel.h"
#include "UI/Panels/NumberPadPopup.h"
#include "UI/Panels/OutputOffStatePopup.h"
#include "UI/Panels/ResistanceRangePopup.h"
#include "UI/Panels/SourcePanel.h"
#include "UI/Panels/SourceRangePopup.h"
#include "UI/Panels/XYGraphPopup.h"

constexpr uint32_t FRAME_TIME_MS = 20;
constexpr uint8_t BEEPER_PIN = 9;
constexpr bool SHOW_MEASURE_PANEL = false;

Container root(0, 0, 800, 480);
MainPage mainPage(0, 0, 800, 480);
MeasurePanel measurePanel(20, 4 + 54 + Theme::PanelGap, 760, 201);
SourceRangePopup sourceRangePopup(0, 0, 800, 480);
ResistanceRangePopup resistanceRangePopup(0, 0, 800, 480);
NumberPadPopup numberPadPopup(0, 0, 800, 480);
MeasureFunctionPopup measureFunctionPopup(0, 0, 800, 480);
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
    measurePanel.SetVisible(SHOW_MEASURE_PANEL);
}

void UpdateCaptionFromMode()
{
    switch (System.GetMeasureMode())
    {
        case MeasureMode::Voltage:
            measurePanel.SetCaption("Voltage");
            break;

        case MeasureMode::Current:
            measurePanel.SetCaption("Current");
            break;

        case MeasureMode::Resistance:
            measurePanel.SetCaption("Resistance");
            break;

        case MeasureMode::Power:
            measurePanel.SetCaption("Power");
            break;

        default:
            measurePanel.SetCaption("Measure");
            break;
    }
}

void OpenMeasureFunctionPopup(Button* sender)
{
    if (sender != &measurePanel.GetFunctionButton())
    {
        return;
    }

    measureFunctionPopup.Open();
}

void OnMeasureFunctionPopupClosed(MeasureFunctionPopup* sender, bool accepted)
{
    if (sender != &measureFunctionPopup)
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
    if (sender == &measurePanel.GetRangeButton())
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
    measurePanel.SetSystem(&System);
    sourceRangePopup.SetSystem(&System);
    resistanceRangePopup.SetSystem(&System);
    numberPadPopup.SetSystem(&System);
    measureFunctionPopup.SetSystem(&System);
    outputOffStatePopup.SetSystem(&System);
    xyGraphPopup.SetSystem(&System);
    measureFunctionPopup.SetOnClose(OnMeasureFunctionPopupClosed);
    SourcePanel& sourcePanel = mainPage.GetSourcePanel();
    sourcePanel.SetOnExpandChanged(OnSourceExpandChanged);
    measurePanel.GetRangeButton().SetOnClick(OpenSourceRangePopup);
    measurePanel.GetFunctionButton().SetOnClick(OpenMeasureFunctionPopup);
    sourcePanel.GetVoltagePanel().GetRangeButton().SetOnClick(OpenSourceRangePopup);
    sourcePanel.GetCurrentPanel().GetRangeButton().SetOnClick(OpenSourceRangePopup);
    sourcePanel.GetVoltagePanel().GetSourceButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetVoltagePanel().GetLimitButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetCurrentPanel().GetSourceButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetCurrentPanel().GetLimitButton().SetOnClick(OpenNumberPadPopup);
    sourcePanel.GetMenuPanel().GetOutputOffStateButton().SetOnClick(OpenOutputOffStatePopup);
    sourcePanel.GetMenuPanel().GetGraphButton().SetOnClick(OpenXYGraphPopup);
    UpdateCaptionFromMode();
    ApplyLayout();

    root.AddChild(&mainPage);

    if (SHOW_MEASURE_PANEL)
    {
        root.AddChild(&measurePanel);
    }

    root.AddChild(&sourceRangePopup);
    root.AddChild(&numberPadPopup);
    root.AddChild(&outputOffStatePopup);
    root.AddChild(&xyGraphPopup);

    if (SHOW_MEASURE_PANEL)
    {
        root.AddChild(&resistanceRangePopup);
        root.AddChild(&measureFunctionPopup);
    }

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
