
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


/*----------------------------------------------------------------
    NumberPad
------------------------------------------------------------------*/

void NumberPad::OnKeyAbort(void) { Display.HideWindow(); }
void NumberPad::OnKey01(void) { AddToBuffer('1'); }
void NumberPad::OnKey02(void) { AddToBuffer('2'); }
void NumberPad::OnKey03(void) { AddToBuffer('3'); }
void NumberPad::OnKey04(void) { AddToBuffer('4'); }
void NumberPad::OnKey05(void) { AddToBuffer('5'); }
void NumberPad::OnKey06(void) { AddToBuffer('6'); }
void NumberPad::OnKey07(void) { AddToBuffer('7'); }
void NumberPad::OnKey08(void) { AddToBuffer('8'); }
void NumberPad::OnKey09(void) { AddToBuffer('9'); }
void NumberPad::OnKey00(void) { AddToBuffer('0'); }

void NumberPad::OnKeyComma(void) { AddDecimalPointToBuffer('.'); }
void NumberPad::OnKeyBackspace(void) { BackspaceToBuffer(); }
void NumberPad::OnKeyClear(void) { Clear(); }
void NumberPad::OnKeyPlusMinus(void) { TogglePlusMinus(); }

void NumberPad::OnKeyUnit(void)
{
    UnitBase = Base;
    BufferToDisplay();
}
void NumberPad::OnKeyUnitMillis(void)
{
    UnitBase = Millis;
    BufferToDisplay();
}
void NumberPad::OnKeyUnitMikros(void)
{
    UnitBase = Mikros;
    BufferToDisplay();
}

NumberPad::NumberPad(INumberPad *device, int x, int y, int w, int h)
{
    //if (device == nullptr)
    //    Serial.println("DEVICE IS NULL");

    X = x;
    Y = y;
    W = w;
    H = h;
    pDevice = device;

    int btnH = 70; // Button Height
    int btnW = 90; // Button Width

    int row = 130; // y-Start
    int col = 30;  // x-Start
    int d = 6;     // Abstand

    InputField = new LabelClass(50, 60, 2, C1, "");

    Btn1 = new ButtonClass(col, row, btnW, btnH, 30, "1");
    col = col + btnW + d;
    Btn2 = new ButtonClass(col, row, btnW, btnH, 30, "2");
    col = col + btnW + d;
    Btn3 = new ButtonClass(col, row, btnW, btnH, 30, "3");
    col = col + btnW + d;

    BtnUnit = new ButtonClass(col, row, btnW, btnH, 30, (pDevice->GetSourceType() == VoltageSourceType) ? "V" : "A");

    col = col + btnW + d;
    BtnAbort = new ButtonClass(col, row, 140, btnH, 30, "Abort");

    row = row + btnH + d;
    col = 30;

    Btn4 = new ButtonClass(col, row, btnW, btnH, 30, "4");
    col = col + btnW + d;
    Btn5 = new ButtonClass(col, row, btnW, btnH, 30, "5");
    col = col + btnW + d;
    Btn6 = new ButtonClass(col, row, btnW, btnH, 30, "6");
    col = col + btnW + d;

    BtnUnitMilli = new ButtonClass(col, row, btnW, btnH, 30, "unit");

    BtnUnitMilli = new ButtonClass(col, row, btnW, btnH, 30, (pDevice->GetSourceType() == VoltageSourceType) ? "mV" : "mA");
    
    col = col + btnW + d;
    BtnClear = new ButtonClass(col, row, 140, btnH, 30, "Clear");

    row = row + btnH + d;
    col = 30;

    Btn7 = new ButtonClass(col, row, btnW, btnH, 30, "7");
    col = col + btnW + d;
    Btn8 = new ButtonClass(col, row, btnW, btnH, 30, "8");
    col = col + btnW + d;
    Btn9 = new ButtonClass(col, row, btnW, btnH, 30, "9");
    col = col + btnW + d;
    
    BtnUnitMicro = new ButtonClass(col, row, btnW, btnH, 30, (pDevice->GetSourceType() == VoltageSourceType) ? "uV" : "uA");
    
    col = col + btnW + d;
    BtnBackspace = new ButtonClass(col, row, 140, btnH, 30, "Back");

    row = row + btnH + d;
    col = 30;

    Btn0 = new ButtonClass(col, row, btnW, btnH, 30, "0");
    col = col + btnW + d;
    BtnPlusMinus = new ButtonClass(col, row, btnW, btnH, 30, "+/-");
    col = col + btnW + d;
    BtnDecimal = new ButtonClass(col, row, btnW, btnH, 30, ".");
    col = col + btnW + d;
    BtnNone = new ButtonClass(col, row, btnW, btnH, 30, " ");
    col = col + btnW + d;
    BtnEnter = new ButtonClass(col, row, 140, btnH, 30, "Enter");

    Clear(); // Buffer
    AddListner();
    AddControls();
}

void NumberPad::Show()
{
    //Serial.println("NumberPad::Show()");
   

    if (pDevice != nullptr)
    {
         float v = pDevice->GetValue();
        SetValue(v);
    } else 
    {
       Serial.println("[Error] NumberPad kat keinen Device");
    }
}

// Übergabe erfolgt in der Base-Auflösung
void NumberPad::SetValue(float v)
{

    switch (UnitBase)
    {
    case Base:
        break;
    case Millis:
        v = v * 1000.0;
        break;
    case Mikros:
        v = v * 1000.0 * 1000.0;
        break;
    }
    InputBuffer = "";

    // TODO Nachkommastellen 4,2
    InputBuffer.concat(v);
    BufferToDisplay();

    /*
        char buf[20];
        char* string = dtostrf(num, 4, 2, buf);
        return concat(string, strlen(string));
    */
}

void NumberPad::AddListner()
{

    Btn0->SetListener(this, (CallbackFn)&NumberPad::OnKey00);
    Btn1->SetListener(this, (CallbackFn)&NumberPad::OnKey01);
    Btn2->SetListener(this, (CallbackFn)&NumberPad::OnKey02);
    Btn3->SetListener(this, (CallbackFn)&NumberPad::OnKey03);
    Btn4->SetListener(this, (CallbackFn)&NumberPad::OnKey04);
    Btn5->SetListener(this, (CallbackFn)&NumberPad::OnKey05);
    Btn6->SetListener(this, (CallbackFn)&NumberPad::OnKey06);
    Btn7->SetListener(this, (CallbackFn)&NumberPad::OnKey07);
    Btn8->SetListener(this, (CallbackFn)&NumberPad::OnKey08);
    Btn9->SetListener(this, (CallbackFn)&NumberPad::OnKey09);

    BtnEnter->SetListener(this, (CallbackFn)&NumberPad::OnKeyEnter);

    BtnAbort->SetListener(this, (CallbackFn)&NumberPad::OnKeyAbort);

    BtnDecimal->SetListener(this, (CallbackFn)&NumberPad::OnKeyComma);
    BtnBackspace->SetListener(this, (CallbackFn)&NumberPad::OnKeyBackspace);
    BtnClear->SetListener(this, (CallbackFn)&NumberPad::OnKeyClear);
    BtnPlusMinus->SetListener(this, (CallbackFn)&NumberPad::OnKeyPlusMinus);

    BtnUnit->SetListener(this, (CallbackFn)&NumberPad::OnKeyUnit);
    BtnUnitMilli->SetListener(this, (CallbackFn)&NumberPad::OnKeyUnitMillis);
    BtnUnitMicro->SetListener(this, (CallbackFn)&NumberPad::OnKeyUnitMikros);
}

void NumberPad::AddControls()
{
    AddControl(BtnAbort);

    AddControl(InputField);

    AddControl(Btn1);
    AddControl(Btn2);
    AddControl(Btn3);
    AddControl(BtnUnit);
    AddControl(Btn4);
    AddControl(Btn5);
    AddControl(Btn6);
    AddControl(BtnUnitMilli);
    AddControl(Btn7);
    AddControl(Btn8);
    AddControl(Btn9);
    AddControl(BtnUnitMicro);
    AddControl(Btn0);
    AddControl(BtnPlusMinus);
    AddControl(BtnDecimal);
    AddControl(BtnBackspace);

    AddControl(BtnEnter);
    AddControl(BtnClear);
    AddControl(BtnNone);
}

// ----- Sollwert setzten

void NumberPad::OnKeyEnter(void)
{

    if (pDevice != nullptr)
    {
        pDevice->SetValue(GetAsFloat());
        Display.HideWindow();
    }
    else
    {
        Serial.println("[Error] NumberPad kat keinen Device");
    }
}

void NumberPad::ValidateInput()
{

    float v = GetAsFloat();

    if (pDevice->ValidateValue(v) == false)
    {

        InputField->SetFontColor(THEME_ERROR_COLOR);
    }
    else
    {
        InputField->SetFontColor(C1);
    }
}

// Eingabefeld im Display aktualisieren
void NumberPad::BufferToDisplay()
{
    ValidateInput();

    
    DisplayBuffer = "";
    DisplayBuffer.concat(InputBuffer.c_str());
    if (UnitBase == Base)
        DisplayBuffer.concat((pDevice->GetSourceType() == VoltageSourceType) ? " V" : " A");
    if (UnitBase == Millis)
        DisplayBuffer.concat((pDevice->GetSourceType() == VoltageSourceType) ? " mV" : " mA");
    if (UnitBase == Mikros)
        DisplayBuffer.concat((pDevice->GetSourceType() == VoltageSourceType) ? " uV" : " uA");

    InputField->SetText(DisplayBuffer.c_str());

    
}

void NumberPad::AddDecimalPointToBuffer(char c)
{
    if (InputBuffer.indexOf(c) == -1)
    {
        InputBuffer.concat(c);
        BufferToDisplay();
    }
}

void NumberPad::TogglePlusMinus()
{
    int idx = -1;
    if ((idx = InputBuffer.indexOf("+")) != -1)
    {
        InputBuffer.replace("+", "-");
    }
    else
    {
        InputBuffer.replace("-", "+");
    }
    BufferToDisplay();
}

void NumberPad::AddToBuffer(char c)
{

    if (InputBuffer.length() < MAX_INPUT_LENGTH)
    {
        InputBuffer.concat(c);
        BufferToDisplay();
    }
}

void NumberPad::BackspaceToBuffer(void)
{
    if (InputBuffer.length() > 1)
    {
        InputBuffer.remove(InputBuffer.length() - 1);
        BufferToDisplay();
    }
}

void NumberPad::Clear()
{
    InputBuffer = "+";
    InputField->SetText("+");
}

float NumberPad::GetAsFloat(void)
{
    InputBuffer.replace(',', '.');
    float v = InputBuffer.toFloat();

    switch (UnitBase)
    {
    case Base:
        break;
    case Millis:
        v = v / 1000.0;
        break;
    case Mikros:
        v = v / (1000.0 * 1000.0);
        break;
    }

    return v;
}

void NumberPad::Render()
{
    // RenderFrame

    // Render Background Frame
    GD.Begin(RECTS);

    GD.ColorRGB(0x808080);

    GD.LineWidth(16 * 2);
    GD.Vertex2f(X, Y);
    GD.Vertex2f((X + W), (Y + H));

    GD.ColorRGB(0x1A3857);

    GD.Vertex2f((X + 2), (Y + 2));
    GD.Vertex2f((X + W - 2), (Y + H - 2));

    // Render Input-Field
    GD.Begin(RECTS);
    GD.ColorRGB(0x808080);

    GD.LineWidth(16 * 2);
    GD.Vertex2f(X + 30, Y + 20);
    GD.Vertex2f((X + 530), (Y + 100));
    GD.ColorRGB(0x000000);

    GD.LineWidth(16 * 2);
    GD.Vertex2f(X + 30 + 2, Y + 20 + 2);
    GD.Vertex2f((X + 530 - 2), (Y + 100 - 2));

    ContainerClass::Render();
}
