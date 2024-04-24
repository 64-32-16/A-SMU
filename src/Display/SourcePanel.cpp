#include "SourcePanel.h"
#include "Display.h"

//----------------------------------------------------------------------
// VoltageSource

VoltageSourcePanel::VoltageSourcePanel(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;

    Title = "VOLTAGE";

    pSourceNumberPad = new NumberPad(System.GetVoltageSource(), 100, 10, 600, 460);

    pLimitNumberPad = new NumberPad(System.GetVoltageSource()->GetLimit(), 100, 10, 600, 460);

    pRangePad = new RangePadClass(  10,180, (800-20) , (480-180-10) , System.GetVoltageMeasurement());

    int startX = THEME_XSTART;
    ModeLabel = new LabelClass(startX, 2, 29, THEME_SECONDARY_COLOR, "SOURCE VOLTAGE");
    ModeLabel->BgColor = THEME_SECONDARY_BGCOLOR;

    MonValue = new LabelClass(startX, 70, 1, THEME_VOLTAGE_LABEL_COLOR, "000.00012 V");

    // Range-Button
    RangeLabel = new LabelClass(startX, 145, 29, THEME_SECONDARY_COLOR, "Range");
    RangeButton = new ButtonClass(startX + 100, 125, "4V");
    RangeButton->SetListener(this, (CallbackFn)&VoltageSourcePanel::OnRangeClick);

    SourceLabel = new LabelClass(startX + 220, 145, 29, THEME_SECONDARY_COLOR, "Source");
    SourceButton = new ButtonClass(startX + 320, 125, "+1.2456V");
    SourceButton->SetListener(this, (CallbackFn)&VoltageSourcePanel::OnSetSourceClick);

    LimitLabel = new LabelClass(startX + 510, 145, 29, THEME_SECONDARY_COLOR, "Limit");
    LimitButton = new ButtonClass(startX + 590, 125, "+1.2345mA");
    LimitButton->SetListener(this, (CallbackFn)&VoltageSourcePanel::OnSetLimitClick);

    AddControl(ModeLabel);
    AddControl(MonValue);

    AddControl(RangeLabel);
    AddControl(RangeButton);

    AddControl(SourceLabel);
    AddControl(SourceButton);

    AddControl(LimitLabel);
    AddControl(LimitButton);
}

void VoltageSourcePanel::OnSetLimitClick()
{
    if (pLimitNumberPad == nullptr)
        return;

    Display.ShowWindow(pLimitNumberPad);
}

void VoltageSourcePanel::OnSetSourceClick()
{
    if (pSourceNumberPad == nullptr)
        return;

    Display.ShowWindow(pSourceNumberPad);
}

void VoltageSourcePanel::OnRangeClick()
{
    if (pRangePad == nullptr)
        return;

    Display.ShowWindow(pRangePad);
}

void VoltageSourcePanel::Render()
{
    if (Visible == false)
        return;

    // Gemessen Wert aus den Buffer anzeigen.
    MonValue->SetText(System.FormatVoltage(System.Buffer.GetVoltage()).c_str());

    // Aktuellen Ranges im Button anzeigen
    RangeClass *range = System.GetVoltageMeasurement()->GetSelectedRange();

    if (range != nullptr)
    {
        if (range->GetRangeType() == Voltage4V)
            RangeButton->SetText("4V");
        if (range->GetRangeType() == Voltage30V)
            RangeButton->SetText("30V");
    }
    else
    {
        RangeButton->SetText("Error");
    }

    // Aktuellen Source-Wert anzeigen,
    float source = System.GetVoltageSource()->GetValue();
    SourceButton->SetText(System.FormatVoltage(source).c_str());

    // Limit anzeigen.
    float limit = System.GetVoltageSource()->GetLimit()->GetValue();
    LimitButton->SetText(System.FormatCurrent(limit).c_str());

    ContainerClass::Render();
}
