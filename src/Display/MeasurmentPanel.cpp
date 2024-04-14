
#include "Display/MeasurmentPanel.h"
#include "Display.h"


//-----------------------------------------------------------------------
// C U R R E N T
//-----------------------------------------------------------------------

CurrentMeasurmentPanel::CurrentMeasurmentPanel(int x, int y, int w, int h) 
: MeasurmentPanel( x,y,w,h)
{

    Title = "CURRENT";
    pCurrentRangePad = new CurrentRangePad(  100,100, 600 , 300 );
    
   

    RangeButton->SetListener( this, (CallbackFn) &CurrentMeasurmentPanel::OnRangeClick);

}


void CurrentMeasurmentPanel::OnRangeClick() 
{   
    Display.ShowWindow(pCurrentRangePad);
} 

void CurrentMeasurmentPanel::Render()
{

    TitleLabel->SetText("MEASURING CURRENT 2-Wire");
    ValueLabel->SetText(System.FormatCurrent(System.Buffer.GetCurrent()).c_str());
    ValueLabel->SetFontColor(THEME_CURRENT_LABEL_COLOR);
    TitleLabel->SetFontColor(THEME_CURRENT_LABEL_COLOR);
    RangeLabel->SetFontColor(THEME_CURRENT_LABEL_COLOR);
   

    
    // Anzeige des aktuellen Ranges im Button
    RangeClass *range =   System.GetCurrentMeasurement()->GetSelectedRange(); 

    if( range != nullptr)
    {
        if( range->GetRangeType() == Current1mA ) RangeButton->SetText("1mA");
        if( range->GetRangeType() == Current100mA ) RangeButton->SetText("100mA");
        if( range->GetRangeType() == Current1A ) RangeButton->SetText("1A");
    } else 
    {
        RangeButton->SetText("Error");
    }
    MeasurmentPanel::Render();

}




//-----------------------------------------------------------------------
// V O L T A G E
//-----------------------------------------------------------------------


VoltageMeasurmentPanel::VoltageMeasurmentPanel(int x, int y, int w, int h) 
: MeasurmentPanel( x,y,w,h)
{

    Title = "VOLTAGE";
    pVoltageRangePad = new VoltageRangePad(  100,100, 600 , 300 );
    
   

    RangeButton->SetListener( this, (CallbackFn) &VoltageMeasurmentPanel::OnRangeClick);

}


void VoltageMeasurmentPanel::OnRangeClick() 
{   
    Display.ShowWindow(pVoltageRangePad);
} 

void VoltageMeasurmentPanel::Render()
{

    TitleLabel->SetText("MEASURING VOLTAGE");
    ValueLabel->SetText(System.FormatVoltage(System.Buffer.GetVoltage()).c_str());
    ValueLabel->SetFontColor(THEME_VOLTAGE_LABEL_COLOR);
    TitleLabel->SetFontColor(THEME_VOLTAGE_LABEL_COLOR);
    RangeLabel->SetFontColor(THEME_VOLTAGE_LABEL_COLOR);


    
    // Anzeige des aktuellen Ranges im Button
    
    RangeClass *range =   System.GetVoltageMeasurement()->GetSelectedRange(); 

    if( range != nullptr)
    {
        if( range->GetRangeType() == Voltage4V ) RangeButton->SetText("4V");
        if( range->GetRangeType() == Voltage30V ) RangeButton->SetText("30V");
    } else 
    {
        RangeButton->SetText("Error");
    }
    
    MeasurmentPanel::Render();

}



//-----------------------------------------------------------------------
// B A S E   C L A S S
//-----------------------------------------------------------------------

MeasurmentPanel::MeasurmentPanel(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;

    int startX = 20;

    TitleLabel = new LabelClass(startX, 2, 29, C1, "MEASURING ....");

    ValueLabel = new LabelClass(startX, 70, 1, C1, "000.00012 ?");

    RangeLabel = new LabelClass(startX, 145, 29, C1, "Range");
    RangeButton = new ButtonClass(startX + 100, 125, "100 ??");

    AddControl(TitleLabel);
    AddControl(ValueLabel);

    AddControl(RangeButton);
    AddControl(RangeLabel);
}



void MeasurmentPanel::Render()
{
    ContainerClass::Render();
}

