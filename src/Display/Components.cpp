
#include "Display/Components.h"
#include "Display.h"

/*----------------------------------------------------------------
 CurrentRangePad
------------------------------------------------------------------*/

CurrentRangePad::CurrentRangePad(  int x, int y, int w, int h) 
{
    	X=x;
        Y=y;
        W=w;
        H=h;
 


        int btnH = 70;  // Button Height
        int btnW = 120;  // Button Width

        int xStart = 40;

        LabelTitle = new LabelClass( 50, 30 ,  24,  C0, "SELECT CURRENT RANGE");

        BtnAuto = new ButtonClass( xStart, 80 , btnW, btnH, 29, "Auto");
        xStart = xStart + btnW + 10;
        BtnAuto->IsDisabled = true;

        BtnRange1mA = new ButtonClass( xStart, 80 , btnW, btnH, 29, "1mA"); 
         xStart = xStart + btnW + 10;

        BtnRange100mA = new ButtonClass( xStart, 80 , btnW, btnH, 29, "100mA");
         xStart = xStart + btnW + 10; 
        BtnRange1A = new ButtonClass( xStart, 80 , btnW, btnH, 29, "1A"); 
         

        BtnClose = new ButtonClass( 200, 200 , 180, 50, 29, "Close"); 

        AddListner();
        AddControls();
}

void CurrentRangePad::OnKeyClose(void) { Display.HideWindow();}
void CurrentRangePad::OnKey1mA(void) 
{    
    System.GetCurrentMeasurement()->SetRange( Current1mA);
}
void CurrentRangePad::OnKey100mA(void) 
{ 
    System.GetCurrentMeasurement()->SetRange( Current100mA);
}
void CurrentRangePad::OnKey1A(void) 
{ 
    System.GetCurrentMeasurement()->SetRange( Current1A);
}


void CurrentRangePad::AddListner() 
{
    BtnClose->SetListener(this, (CallbackFn) &CurrentRangePad::OnKeyClose );
    BtnRange1mA->SetListener(this, (CallbackFn) &CurrentRangePad::OnKey1mA );
    BtnRange100mA->SetListener(this, (CallbackFn) &CurrentRangePad::OnKey100mA );
    BtnRange1A->SetListener(this, (CallbackFn) &CurrentRangePad::OnKey1A );
}

void CurrentRangePad::Show() 
{
    RangeType t = System.GetCurrentMeasurement()->GetSelectedRange()->GetRangeType();
    if( t == Current1mA) Display.SetFocus(BtnRange1mA);
    if( t == Current100mA) Display.SetFocus(BtnRange100mA);
    if( t == Current1A) Display.SetFocus(BtnRange1A);
    


}

void CurrentRangePad::AddControls() 
{
     AddControl(LabelTitle);
     AddControl( BtnAuto);
     AddControl( BtnRange1mA);
     AddControl( BtnRange100mA);
     AddControl( BtnRange1A);
     AddControl( BtnClose);
     
     
}


void CurrentRangePad::Render() 
{
    
    // Render Background Frame
    GD.Begin(RECTS);
   
    GD.ColorRGB(0x808080);
   
    GD.LineWidth(16*2);
    GD.Vertex2f( X, Y );
    GD.Vertex2f( (X+W), (Y+H));


    GD.ColorRGB(0x1A3857);

    GD.Vertex2f( (X+2), (Y+2));
    GD.Vertex2f( (X+W-2), (Y+H-2));

    ContainerClass::Render();

}


/*----------------------------------------------------------------
    VoltageRangePad
------------------------------------------------------------------*/

VoltageRangePad::VoltageRangePad(  int x, int y, int w, int h) 
{
    	X=x;
        Y=y;
        W=w;
        H=h;



        int btnH = 70;  // Button Height
        int btnW = 120;  // Button Width

        int xStart = 40;

        LabelTitle = new LabelClass( 50, 30 ,  24,  C0, "SELECT VOLTAGE RANGE");

        BtnAuto = new ButtonClass( xStart, 80 , btnW, btnH, 29, "Auto");
        xStart = xStart + btnW + 10;
        BtnAuto->IsDisabled = true;

        BtnRange200mV = new ButtonClass( xStart, 80 , btnW, btnH, 29, "200mV"); 
         xStart = xStart + btnW + 10;
        BtnRange200mV->IsDisabled = true;

        BtnRange4V = new ButtonClass( xStart, 80 , btnW, btnH, 29, "4V");
         xStart = xStart + btnW + 10; 
        BtnRange30V = new ButtonClass( xStart, 80 , btnW, btnH, 29, "30V"); 
         

        BtnClose = new ButtonClass( 200, 200 , 180, 50, 29, "Close"); 

        AddListner();
        AddControls();
}

void VoltageRangePad::OnKeyClose(void) { Display.HideWindow();}

void VoltageRangePad::OnKey4V(void) 
{    
    System.GetVoltageMeasurement()->SetRange( Voltage4V);
}
void VoltageRangePad::OnKey30V(void) 
{    
    System.GetVoltageMeasurement()->SetRange( Voltage30V);
}

void VoltageRangePad::AddListner() 
{
    
    BtnClose->SetListener(this, (CallbackFn) &VoltageRangePad::OnKeyClose );
    BtnRange4V->SetListener(this, (CallbackFn) &VoltageRangePad::OnKey4V );
    BtnRange30V->SetListener(this, (CallbackFn) &VoltageRangePad::OnKey30V );
}

void VoltageRangePad::Show() 
{
    RangeType t = System.GetVoltageMeasurement()->GetSelectedRange()->GetRangeType();
    if( t == Voltage4V) Display.SetFocus(BtnRange4V);
    if( t == Voltage30V) Display.SetFocus(BtnRange30V);
}

void VoltageRangePad::AddControls() 
{
     AddControl(LabelTitle);
     AddControl( BtnAuto);
     AddControl( BtnRange200mV);
     AddControl( BtnRange4V);
     AddControl( BtnRange30V);
     AddControl( BtnClose);
     
}


void VoltageRangePad::Render() 
{
    
    // Render Background Frame
    GD.Begin(RECTS);
   
    GD.ColorRGB(0x808080);
   
    GD.LineWidth(16*2);
    GD.Vertex2f( X, Y );
    GD.Vertex2f( (X+W), (Y+H));


    GD.ColorRGB(0x1A3857);

    GD.Vertex2f( (X+2), (Y+2));
    GD.Vertex2f( (X+W-2), (Y+H-2));

    ContainerClass::Render();

}


