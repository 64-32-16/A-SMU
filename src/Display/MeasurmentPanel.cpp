
#include "Display/MeasurmentPanel.h"
#include "Display.h"



//-----------------------------------------------------------------------
// C U R R E N T
//-----------------------------------------------------------------------

CurrentMeasurmentPanel::CurrentMeasurmentPanel(int x, int y, int w, int h) 
: MeasurmentPanel( x,y,w,h)
{

    Title = "CURRENT";
   
    pRangePad = new RangePadClass(  10,180, (800-20) , (480-180-10) , System.GetCurrentMeasurement());
   
   
    RangeButton->SetListener( this, (CallbackFn) &CurrentMeasurmentPanel::OnRangeClick);

}

/**
 * @brief show Range Dialog Window
 * 
 */
void CurrentMeasurmentPanel::OnRangeClick() 
{   
    Display.ShowWindow(pRangePad);
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
    MeasurmentPanel::SetRage( range);

    MeasurmentPanel::Render();

}




//-----------------------------------------------------------------------
// V O L T A G E
//-----------------------------------------------------------------------


VoltageMeasurmentPanel::VoltageMeasurmentPanel(int x, int y, int w, int h) 
: MeasurmentPanel( x,y,w,h)
{

    Title = "VOLTAGE";
    
    pRangePad = new RangePadClass(  10,180, (800-20) , (480-180-10) , System.GetVoltageMeasurement());
   

    RangeButton->SetListener( this, (CallbackFn) &VoltageMeasurmentPanel::OnRangeClick);

}


void VoltageMeasurmentPanel::OnRangeClick() 
{   
    Display.ShowWindow(pRangePad);
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
    MeasurmentPanel::SetRage( range);

    
    MeasurmentPanel::Render();

}


//-----------------------------------------------------------------------
//  R E S I S T O R
//-----------------------------------------------------------------------


ResistorMeasurmentPanel::ResistorMeasurmentPanel(int x, int y, int w, int h) 
: MeasurmentPanel( x,y,w,h)
{

    Title = "RESISTOR";
   
    pRangePad = new RangePadClass(  10,180, (800-20) , (480-180-10) , System.GetResistorMeasurement());
    
   
    RangeButton->SetListener( this, (CallbackFn) &ResistorMeasurmentPanel::OnRangeClick);

}

/**
 * @brief show Range Dialog Window
 * 
 */
void ResistorMeasurmentPanel::OnRangeClick() 
{   
    Display.ShowWindow(pRangePad);
} 



void ResistorMeasurmentPanel::Render()
{

    TitleLabel->SetText("MEASURING RESISTOR 2-Wire");
    

    ValueLabel->SetText(System.FormatResistor(System.Buffer.GetResistor()).c_str());
    
    ValueLabel->SetFontColor(THEME_RESISTOR_LABEL_COLOR);
    TitleLabel->SetFontColor(THEME_RESISTOR_LABEL_COLOR);
    RangeLabel->SetFontColor(THEME_RESISTOR_LABEL_COLOR);
   

    
    // Anzeige des aktuellen Ranges im Button
    RangeClass *range =   System.GetResistorMeasurement()->GetSelectedRange(); 
    MeasurmentPanel::SetRage( range);

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

    pFunctionPad = new MeasuringFunctionPadClass(  100,30, (600) , (480-20-20) );
    
    TitleLabel = new LabelClass(startX, 2, 29, C1, "..");

    ValueLabel = new LabelClass(startX, 70, 1, C1, "..");


    RangeLabel = new LabelClass(startX, 145, 29, C1, "Range");

    RangeButton = new ButtonClass(startX + 100, 125, "..");
    FunctionButton = new ButtonClass(startX + 600, 125, "Function");
    
    FunctionButton->SetListener( this, (CallbackFn) &MeasurmentPanel::OnFunctionClick);



    AddControl(TitleLabel);
    AddControl(ValueLabel);

    AddControl(FunctionButton);
    AddControl(RangeButton);
    AddControl(RangeLabel);
}

void MeasurmentPanel::OnFunctionClick() 
{   
    Display.ShowWindow(pFunctionPad);
} 

void MeasurmentPanel::SetRage(RangeClass *range )
{
    if( range != nullptr)
    {
        RangeButton->SetText( range->GetText());
       
    } else 
    {
        RangeButton->SetText("Error");
    }
}



void MeasurmentPanel::Render()
{
    ContainerClass::Render();
}

