
#include "Display/Components.h"
#include "Display.h"




/*----------------------------------------------------------------
 WirePadClass
------------------------------------------------------------------*/

/**
 * @brief
 *
 */
WirePadClass::WirePadClass(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;
    int xStart = 50;
    int btnH = 50;  // Button Height
    int btnW = 220; // Button Width

  

    LabelTitle = new LabelClass(xStart, 30, 24, C0, "Wiring");
    BtnClose = new ButtonClass(70, 360, 180, 50, 29, "Close");

    Btn2Wire = new ButtonClass(xStart, 60, btnW, btnH, 29, "2-Wire");
    Btn4Wire = new ButtonClass(xStart, 120 + 10, btnW, btnH, 29, "4-Wire");


    BtnClose->SetClickEvent(this, (OnBtnClickFn)&WirePadClass::OnKeyClose);
    Btn2Wire->SetClickEvent(this, (OnBtnClickFn)&WirePadClass::OnKey2Wire);
    Btn4Wire->SetClickEvent(this, (OnBtnClickFn)&WirePadClass::OnKey4Wire);
 
    AddControl(BtnClose);
    AddControl(LabelTitle);
    AddControl(Btn2Wire);
    AddControl(Btn4Wire);
}

void WirePadClass::OnKey2Wire(ButtonClass *btn)
{
    System.SetSelectedWire(Wire2);
    OnKeyClose(btn);
}
void WirePadClass::OnKey4Wire(ButtonClass *btn)
{
     System.SetSelectedWire(Wire4);
    OnKeyClose(btn);
}


void WirePadClass::OnKeyClose(ButtonClass *btn) { Display.HideWindow(); }

void WirePadClass::Render()
{

    // Render Background Frame
    GD.Begin(RECTS);

    GD.ColorRGB(0xEEEEEE);

    GD.LineWidth(16 * 4);
    GD.Vertex2f(X, Y);
    GD.Vertex2f((X + W), (Y + H));

    GD.ColorRGB(0x1A3857);

    GD.Vertex2f((X + 2), (Y + 2));
    GD.Vertex2f((X + W - 2), (Y + H - 2));

    ContainerClass::Render();
}

void WirePadClass::Show()
{

    WireType w = System.GetSelectedWire();

    Serial.print("Selected Wire ");Serial.println( w);
    if( w == Wire2 )  Display.SetFocus(Btn2Wire);
    if( w == Wire4 )  Display.SetFocus(Btn4Wire);

}





/*----------------------------------------------------------------
 MeasuringFunctionPadClass
------------------------------------------------------------------*/

/**
 * @brief
 *
 */
MeasuringFunctionPadClass::MeasuringFunctionPadClass(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;
    int xStart = 50;
    int btnH = 50;  // Button Height
    int btnW = 220; // Button Width

    LabelTitle = new LabelClass(xStart, 30, 24, C0, "Measuring");
    BtnClose = new ButtonClass(70, 360, 180, 50, 29, "Close");

    BtnCurrent = new ButtonClass(xStart, 60, btnW, btnH, 29, "Current");
    BtnVoltage = new ButtonClass(xStart, 120 + 10, btnW, btnH, 29, "Voltage");
    BtnResistor = new ButtonClass(xStart, 180 + 20, btnW, btnH, 29, "Resistor");
    BtnPower = new ButtonClass(xStart, 240 + 30, btnW, btnH, 29, "Power");

    BtnClose->SetClickEvent(this, (OnBtnClickFn)&MeasuringFunctionPadClass::OnKeyClose);
    BtnCurrent->SetClickEvent(this, (OnBtnClickFn)&MeasuringFunctionPadClass::OnKeyCurrent);
    BtnVoltage->SetClickEvent(this, (OnBtnClickFn)&MeasuringFunctionPadClass::OnKeyVoltage);
    BtnResistor->SetClickEvent(this, (OnBtnClickFn)&MeasuringFunctionPadClass::OnKeyResistor);
    BtnPower->SetClickEvent(this, (OnBtnClickFn)&MeasuringFunctionPadClass::OnKeyPower);

    AddControl(BtnClose);
    AddControl(LabelTitle);
    AddControl(BtnCurrent);
    AddControl(BtnVoltage);
    AddControl(BtnResistor);
    AddControl(BtnPower);
}

void MeasuringFunctionPadClass::OnKeyCurrent(ButtonClass *btn)
{
    System.SetSelectedMeasuring(CurrentType);
    OnKeyClose(btn);
}
void MeasuringFunctionPadClass::OnKeyVoltage(ButtonClass *btn)
{
    System.SetSelectedMeasuring(VoltageType);
    OnKeyClose(btn);
}

void MeasuringFunctionPadClass::OnKeyResistor(ButtonClass *btn)
{
    System.SetSelectedMeasuring(ResistorType);
    OnKeyClose(btn);
}
void MeasuringFunctionPadClass::OnKeyPower(ButtonClass *btn)
{
    System.SetSelectedMeasuring(PowerType);
    OnKeyClose(btn);
}

void MeasuringFunctionPadClass::OnKeyClose(ButtonClass *btn) { Display.HideWindow(); }

void MeasuringFunctionPadClass::Render()
{

    // Render Background Frame
    GD.Begin(RECTS);

    GD.ColorRGB(0xEEEEEE);

    GD.LineWidth(16 * 4);
    GD.Vertex2f(X, Y);
    GD.Vertex2f((X + W), (Y + H));

    GD.ColorRGB(0x1A3857);

    GD.Vertex2f((X + 2), (Y + 2));
    GD.Vertex2f((X + W - 2), (Y + H - 2));

    ContainerClass::Render();
}

void MeasuringFunctionPadClass::Show()
{
    MeasurementClass *m = System.GetSelectedMeasurement();
    if (m != nullptr)
    {
        switch (m->Type)
        {
        case CurrentType:
            Display.SetFocus(BtnCurrent);
            break;
        case VoltageType:
            Display.SetFocus(BtnVoltage);
            break;
        case ResistorType:
            Display.SetFocus(BtnResistor);
            break;
        case PowerType:
            Display.SetFocus(BtnPower);
            break;
        default:
            break;
        }
    }
}

/*----------------------------------------------------------------
 RangePad
------------------------------------------------------------------*/

/**
 * @brief a rangepad can connect to measuring switch the range
 *
 */
RangePadClass::RangePadClass(int x, int y, int w, int h, MeasurementClass *measuring)
{
    X = x;
    Y = y;
    W = w;
    H = h;
    pMeasuring = measuring;
    Init(measuring);

    LabelTitle = new LabelClass(50, 30, 24, C0, "Measure Range");
    BtnClose = new ButtonClass(280, 220, 180, 50, 29, "Close");

    BtnClose->SetListener(this, (CallbackFn)&RangePadClass::OnKeyClose);

    AddControl(BtnClose);
    AddControl(LabelTitle);
}

void RangePadClass::Init(MeasurementClass *measuring)
{
    int xStart = 40;
    int btnH = 60;  // Button Height
    int btnW = 120; // Button Width
    int yStart = 60;

    if (measuring != nullptr)
    {
        for (int i = 0; i <= measuring->GetRangeCount() - 1; i++)
        {
            RangeClass *r = measuring->GetRange(i);
            if (r != nullptr)
            {
                // Neue Zeile für die Button
                if (xStart + btnW > (800 - 10))
                {
                    yStart = yStart + 80;
                    xStart = 40;
                }

                ButtonClass *btn = new ButtonClass(xStart, yStart, btnW, btnH, 29, r->GetText());
                RangeItems[i] = {r, btn};

                RangeItemsCount = RangeItemsCount + 1;

                btn->SetClickEvent(this, (OnBtnClickFn)&RangePadClass::OnRangeBtnClick);
                AddControl(btn);
                xStart = xStart + btnW + 10;
            }
        }
    }
}

void RangePadClass::OnRangeBtnClick(ButtonClass *btn)
{

    RangeClass *r = GetRangeByBtn(btn);

    if (r != nullptr && pMeasuring != nullptr)
    {
        pMeasuring->SetRange(r->GetRangeType());
        OnKeyClose();
    }
}

RangeClass *RangePadClass::GetRangeByBtn(ButtonClass *btn)
{
    if (btn != nullptr)
    {
        for (int i = 0; i <= RangeItemsCount - 1; i++)
        {
            ButtonClass *b = RangeItems[i].Btn;

            if (b != nullptr)
            {

                if (b == btn) // Compare Pointer
                {
                    return RangeItems[i].Range;
                }
            }
        }
    }

    return nullptr;
}

ButtonClass *RangePadClass::GetBtnByRange(RangeClass *r)
{
    if (r != nullptr)
    {
        for (int i = 0; i <= RangeItemsCount - 1; i++)
        {
            RangeClass *range = RangeItems[i].Range;
            if (range != nullptr && (r == range))
            {
                return RangeItems[i].Btn;
            }
        }
    }

    return nullptr;
}

void RangePadClass::Render()
{

    // Render Background Frame
    GD.Begin(RECTS);

    GD.ColorRGB(0xEEEEEE);

    GD.LineWidth(16 * 4);
    GD.Vertex2f(X, Y);
    GD.Vertex2f((X + W), (Y + H));

    GD.ColorRGB(0x1A3857);

    GD.Vertex2f((X + 2), (Y + 2));
    GD.Vertex2f((X + W - 2), (Y + H - 2));

    ContainerClass::Render();
}

/**
 * @brief the function ist called when the dialog is displayed
 *
 */
void RangePadClass::Show()
{

    if (pMeasuring != nullptr)
    {
        // set focus to selected range buttron
        RangeClass *pRange = pMeasuring->GetSelectedRange();
        if (pRange != nullptr)
        {
            ButtonClass *btn = GetBtnByRange(pRange);
            if (btn != nullptr)
            {
                Display.SetFocus(btn);
            }
        }
    }
}

void RangePadClass::OnKeyClose(void) { Display.HideWindow(); }
