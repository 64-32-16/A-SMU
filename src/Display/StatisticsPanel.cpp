
#include "StatisticsPanel.h"
#include "Display.h"




//----------------------------------------------------------
// StatisticsPanel
//----------------------------------------------------------


void StatisticsPanel::OnClearBtn()
{
    System.Buffer.Reset();
}

StatisticsPanel::StatisticsPanel(int x, int y, int w, int h)
{

    X = x;
    Y = y;
    W = w;
    H = h;

    Title = "STATISTICS";

    int cLegende = THEME_SECONDARY_COLOR;
    int px = 20;
    int py = 10;

    Value = new LabelClass(px, py, 29, cLegende, "Value:");
    py = py + 35;
    PeakToPeak = new LabelClass(px, py, 29, cLegende, "Peak:");
    py = py + 35;
    Average = new LabelClass(px, py, 29, cLegende, "Average:");
    py = py + 35;
    Min = new LabelClass(px, py, 29, cLegende, "Min:");
    py = py + 35;
    Max = new LabelClass(px, py, 29, cLegende, "Max:");
    py = py + 35;

    int c = C1;
    px = px + 120;
    py = 10;
    // Current
    CValue = new LabelClass(px, py, 29, c, "0.12345 A");
    py = py + 35;
    CPeakToPeak = new LabelClass(px, py, 29, c, "0.12345 A");
    py = py + 35;
    CAverage = new LabelClass(px, py, 29, c, "0.12345 A");
    py = py + 35;
    CMin = new LabelClass(px, py, 29, c, "0.12345 A");
    py = py + 35;
    CMax = new LabelClass(px, py, 29, c, "0.12345 A");
    py = py + 35;

    c = 0x79a9fc;
    px = px + 180;
    py = 10;
    // Voltage
    VValue = new LabelClass(px, py, 29, c, "0.12345 V");
    py = py + 35;
    VPeakToPeak = new LabelClass(px, py, 29, c, "0.12345 V");
    py = py + 35;
    VAverage = new LabelClass(px, py, 29, c, "0.12345 V");
    py = py + 35;
    VMin = new LabelClass(px, py, 29, c, "0.12345 V");
    py = py + 35;
    VMax = new LabelClass(px, py, 29, c, "0.12345 V");
    py = py + 35;

    ClearBufferBtn = new ButtonClass(540, 110, 220, 50, 29, "Clear Buffer");
    ClearBufferBtn->SetListener(this, (CallbackFn)&StatisticsPanel::OnClearBtn);

    BufferInfoLabel = new LabelClass(540, 10, 29, cLegende, "Readings:");
    BufferInfo = new LabelClass(680, 10, 29, 0xFFFFFF, "0001");

    AddControl(Value);
    AddControl(PeakToPeak);
    AddControl(Average);
    AddControl(Min);
    AddControl(Max);

    AddControl(CValue);
    AddControl(CPeakToPeak);
    AddControl(CAverage);
    AddControl(CMin);
    AddControl(CMax);

    AddControl(VValue);
    AddControl(VPeakToPeak);
    AddControl(VAverage);
    AddControl(VMin);
    AddControl(VMax);

    AddControl(ClearBufferBtn);
    AddControl(BufferInfoLabel);
    AddControl(BufferInfo);
}

void StatisticsPanel::Render()
{
    if (Visible == false)
        return;

   

    CValue->SetText(System.FormatCurrent(System.Buffer.GetCurrent()).c_str());
    CPeakToPeak->SetText(System.FormatCurrent(System.Buffer.GetCurrentPeakToPeak()).c_str());
    CMin->SetText(System.FormatCurrent(System.Buffer.GetCurrentMin()).c_str());
    CMax->SetText(System.FormatCurrent(System.Buffer.GetCurrentMax()).c_str());
    CAverage->SetText(System.FormatCurrent(System.Buffer.GetCurrentAverage()).c_str());

    VValue->SetText(System.FormatVoltage(System.Buffer.GetVoltage()).c_str());
    VPeakToPeak->SetText(System.FormatVoltage(System.Buffer.GetVoltagePeakToPeak()).c_str());
    VMin->SetText(System.FormatVoltage(System.Buffer.GetVoltageMin()).c_str());
    VMax->SetText(System.FormatVoltage(System.Buffer.GetVoltageMax()).c_str());
    VAverage->SetText(System.FormatVoltage(System.Buffer.GetVoltageAverage()).c_str());

    BufferInfo->SetText(System.FormatBufferCount().c_str());

    ContainerClass::Render();
}

