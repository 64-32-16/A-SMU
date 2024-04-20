
#include "Display/Components.h"
#include "Display.h"

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

    LabelTitle = new LabelClass(50, 30, 24, C0, "GENERIC RANGE PAD");
    BtnClose = new ButtonClass(280, 220, 180, 50, 29, "Close");

    BtnClose->SetListener(this, (CallbackFn)&RangePadClass::OnKeyClose);

    AddControl(BtnClose);
    AddControl(LabelTitle);
}

void RangePadClass::Init(MeasurementClass *measuring)
{
    int xStart = 40;
    int btnH = 70;  // Button Height
    int btnW = 120; // Button Width

    if (measuring != nullptr)
    {
        for (int i = 0; i <= measuring->GetRangeCount() - 1; i++)
        {
            RangeClass *r = measuring->GetRange(i);
            if (r != nullptr)
            {
                ButtonClass *btn = new ButtonClass(xStart, 80, btnW, btnH, 29, r->GetText());
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

    GD.ColorRGB(0x808080);

    GD.LineWidth(16 * 2);
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
