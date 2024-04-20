
#include "display.h"

#include "Beeper.h"



DisplayClass::DisplayClass() 
{
    X=0;
    Y=0;
    W=800;
    H=480;

    pPrimary = new PrimaryPanel(0,0, 800, 240);

    pSecondary = new SecondaryPanel(0,240, 800, 240);

    AddControl( pPrimary);
    AddControl( pSecondary);
    
    Beeper.KeyPress();
    
   

}

void DisplayClass::ShowWindow(WindowClass *p) 
{
    pModalWindow = p; 
  

    if( pModalWindow != nullptr) 
    {
        HideFocus();
        pModalWindow->Show(); 
    }
}


void DisplayClass::Render() 
{
   
   
   ContainerClass::Render();
   
    if( pModalWindow != nullptr) 
    {         
        pModalWindow->Render();
    } 

   RenderFocus(); 
 
}



void DisplayClass::RenderFocus() 
{


    uint32_t  c = 0xf2ea0a;
    if( pFocus != nullptr ) {

        int x = (pFocus->GetX()+1);
        int x1 =  (pFocus->GetX()+pFocus->W-1);
        int y = (pFocus->GetY()+1);
        int y1 = ((pFocus->GetY() + +pFocus->H  -1) );


        GD.Begin(LINE_STRIP);
        GD.LineWidth(1 * 16);
        GD.ColorRGB(c);
        GD.Vertex2f( x , y);
        GD.Vertex2f( x1 , y );
        GD.Vertex2f( x1 , y1 );
        GD.Vertex2f( x , y1 );
        GD.Vertex2f( x , y);
        
                
    }
    
}



void DisplayClass::OnTouchStart(int x, int y) 
{
    Serial.println("DisplayClass::OnTouchStart");
    
    if( pModalWindow != nullptr) 
    {
        pFocus = pModalWindow->FindFocusControl(x, y );    
        pModalWindow->DispatchTouchEvent( TOUCH_START, x, y, 0,0); 


    } else {

        pFocus = FindFocusControl(x, y );    
        DispatchTouchEvent( TOUCH_START, x, y, 0,0); 
    }
}
void DisplayClass::OnTouchEnd(int x, int y) 
{
    if( pModalWindow != nullptr) 
    {
    
        pModalWindow->DispatchTouchEvent( TOUCH_END, x, y,0,0); 
    } else 
    {
        DispatchTouchEvent( TOUCH_END, x, y,0,0); 
        
    }
} 
void DisplayClass::OnTouchSwipe(int x, int y, int dx, int dy)
{
    pFocus = nullptr;

    if( pModalWindow != nullptr) 
    {
        pModalWindow->DispatchTouchEvent( TOUCH_SWIPE, x, y , dx ,dy); 
    } else 
    {
        DispatchTouchEvent( TOUCH_SWIPE, x, y , dx ,dy); 
    }    
} 


void DisplayClass::ReadInput() 
{
    GD.get_inputs();

    if( GD.inputs.tag) 
    {    
        
        if( Input.TouchState == TOUCH_NONE ) 
        {
            Input.StartX = GD.inputs.tag_x; //Start Position merken
            Input.StartY = GD.inputs.tag_y;
            OnTouchStart(GD.inputs.tag_x, GD.inputs.tag_y );
            Input.TouchState = TOUCH_SWIPE;
            
        }


        if(Input.TouchState == TOUCH_SWIPE )        
        {
            int dx = (Input.StartX -  GD.inputs.tag_x);
            int dy = (Input.StartY - GD.inputs.tag_y); 
            if( ( abs(dx)>= 20) || ( abs(dy)>= 20)  ) 
            {
                OnTouchSwipe(GD.inputs.tag_x, GD.inputs.tag_y , dx ,dy );
            } 

        }

    } else 
    {
        if( Input.TouchState == TOUCH_SWIPE) 
        {
            OnTouchEnd(Input.StartX , Input.StartY  );           
        }
        Input.TouchState  = TOUCH_NONE;
    }



}


//********************************************************************
//	GUI
//********************************************************************


HeaderPanel::HeaderPanel(int x, int y ,int w ,int h) 
{
    X=x;
    Y=y;
    W=w;
    H=h;

    int startX=20;
    RemoteLabel = new LabelClass( startX, 16 ,  29,  C0, "Local");
    SystemInfo = new LabelClass( 800-200, 16 ,  29,  C0, "A-SMU V0.1");



    AddControl(RemoteLabel);
    AddControl(SystemInfo);
}


void HeaderPanel::Render() 
{
    
    // Header Frist ROW
    int ROW = 32;

    // First Row
    GD.LineWidth(16 * 1);
    GD.Begin(LINES);
    GD.ColorRGB(THEME_PRIMARAY_COLOR);
    GD.Vertex2f(0,  ROW);
    GD.Vertex2f(800,   ROW);

    ContainerClass::Render();
}


/*
  WIRD ZU SOURCE PANEL !!
*/

PrimaryPanel::PrimaryPanel(int x, int y ,int w ,int h) 
{
    X=x;
    Y=y;
    W=w;
    H=h;

    int startX=20;

    Header = new HeaderPanel( 0,0, 800, 32);

    ModeLabel = new LabelClass( startX, 48, 29, C1, "MEASURING CURRENT");
    
    CurrentMONLabel = new LabelClass(startX, 110, 1,  C1, "000.00012 A"); 

    
    RangeLabel = new LabelClass( startX, 200, 29 , C1, "Range");
    RangeButton = new ButtonClass( startX+100, 180 , "100mA"); 
    RangeButton->SetListener( this, (CallbackFn) &PrimaryPanel::OnRangeClick);

    
    /// TODO ***************************
    // pCurrentRangePad = new CurrentRangePad(  100,100, 600 , 300 );
  
    


     AddControl(Header);
    AddControl(ModeLabel);
    AddControl(CurrentMONLabel);

    AddControl(RangeButton);
    AddControl(RangeLabel);

}

void PrimaryPanel::OnRangeClick() 
{   
    // Display.ShowWindow(pCurrentRangePad);
} 



void PrimaryPanel::Render() 
{

    CurrentMONLabel->SetText( System.FormatCurrent( System.Buffer.GetCurrent()).c_str()); 


    
    // CurrentPanel
    //----------------
    // Anzeige des aktuellen Ranges im Button
    RangeClass *range =   System.GetCurrentMeasurement()->GetSelectedRange(); // System.GetCurrentMeasurement()->GetSelectedRange(); 

    if( range != nullptr)
    {
        if( range->GetRangeType() == Current1mA ) RangeButton->SetText("1mA");
        if( range->GetRangeType() == Current100mA ) RangeButton->SetText("100mA");
        if( range->GetRangeType() == Current1A ) RangeButton->SetText("1A");
    } else 
    {
        RangeButton->SetText("Error");
    }


    ContainerClass::Render();

}

//--------------------------------------------
// Secondary Panel
//--------------------------------------------

SecondaryPanel::SecondaryPanel(int x, int y ,int w ,int h) 
{
    X=x;
    Y=y;
    W=w;
    H=h;


    SwipeController = new SwipeControllerClass();

    TitleLabel = new LabelClass( THEME_XSTART, 16,29, THEME_SECONDARY_COLOR, "Title");
    TitleLabel->BgColor = THEME_SECONDARY_BGCOLOR;
    
  
  
    CurrentMeasure = new CurrentMeasurmentPanel( 0, 50 , 800, 240 - 42);
    VoltageMeasure = new VoltageMeasurmentPanel( 0, 50 , 800, 240 - 42);

    Graph = new GraphClass    ( 0, 50 , 800, 240 - 42);
    Statistics = new StatisticsPanel ( 0, 50 , 800, 240 - 42);
   

    SwipeController->Add( CurrentMeasure);
    SwipeController->Add( VoltageMeasure);
    //SwipeController->Add(Graph);   
    SwipeController->Add(Statistics);
    

    AddControl( TitleLabel); 
    AddControl(  CurrentMeasure);
     AddControl(  VoltageMeasure);
    //AddControl( Graph); 
    AddControl( Statistics); 
  
    SwipeController->SetCurrent(0);

    
}


void SecondaryPanel::TouchSwipe(int x, int y, int dx ,int dy) 
{

    SwipeController->TouchSwipe(x,y, dx, dy);

} 


void SecondaryPanel::Render() 
{

    // HEADER
    //----------------------------------------------

    // Render Background
    GD.Begin(RECTS);
    GD.LineWidth(16 * 2);
    GD.ColorRGB(THEME_SECONDARY_BGCOLOR);

    GD.Vertex2f(GetX() , GetY() );
    GD.Vertex2f((GetX()+W) , (GetY()+H) );

    int ROW_HEIGHT = 32;

    // First Row
    GD.LineWidth(16 * 1);
    GD.Begin(LINE_STRIP);
    GD.ColorRGB(THEME_SECONDARY_COLOR);

    GD.Vertex2f( GetX()  , (GetY()+ ROW_HEIGHT)  );
    GD.Vertex2f( 800 ,  (GetY()+ ROW_HEIGHT) );


    // Render Tabs
    int count = SwipeController->GetCount();
    int distance = 30; // in px
    int x = 400 - (distance * count/2 );

    SetHeaderTitle(); 

    for( int i=0; i< count ;i++ ) 
    {
        
        DrawBall(x+(i* distance), GetY()+16, SwipeController->Current() == i  );
    }

    //-----------------------------


   ContainerClass::Render();
    
}

void SecondaryPanel::SetHeaderTitle() 
{
    TitleLabel->SetText("Secondary");
    int cpIdx = SwipeController->Current();
    ContainerClass *p = SwipeController->GetPanelByIndex( cpIdx );
    if( p != nullptr) TitleLabel->SetText(p->Title);

} 



void SecondaryPanel::DrawBall(int x, int y, bool set) {
  GD.Begin(POINTS);
  GD.PointSize(16 * 6);  
  if (set == false) {
    GD.ColorRGB(255,255,255); 
  } else {
    GD.ColorRGB(0,0,0); 
  }
  GD.Vertex2ii(x, y);
}

void StatisticsPanel::OnClearBtn() 
{
    System.Buffer.Reset();
}

StatisticsPanel::StatisticsPanel(int x, int y ,int w ,int h)
{

    X=x;
    Y=y;
    W=w;
    H=h;

    Title = "STATISTICS";

 
    int cLegende =  THEME_SECONDARY_COLOR;
    int px = 20;
    int py = 10;

    Value = new LabelClass( px, py, 29 , cLegende, "Value:"); py = py + 35;
    PeakToPeak = new LabelClass( px, py, 29 , cLegende, "Peak:"); py = py + 35;
    Average = new LabelClass( px, py, 29 , cLegende, "Average:"); py = py + 35;
    Min = new LabelClass( px, py, 29 , cLegende, "Min:"); py = py + 35;
    Max = new LabelClass( px, py, 29 , cLegende, "Max:"); py = py + 35;

    int c = C1;
    px = px + 120;
    py = 10;
    // Current
    CValue = new LabelClass( px, py, 29 , c, "0.12345 A"); py = py + 35;
    CPeakToPeak = new LabelClass( px, py, 29 , c, "0.12345 A"); py = py + 35;
    CAverage = new LabelClass( px, py, 29 , c, "0.12345 A"); py = py + 35;
    CMin = new LabelClass( px, py, 29 , c, "0.12345 A"); py = py + 35;
    CMax = new LabelClass( px, py, 29 , c, "0.12345 A"); py = py + 35;


    c = 0x79a9fc;
    px = px + 180;
    py = 10;
    // Voltage
    VValue = new LabelClass( px, py, 29 , c, "0.12345 V"); py = py + 35;
    VPeakToPeak = new LabelClass( px, py, 29 , c, "0.12345 V"); py = py + 35;
    VAverage = new LabelClass( px, py, 29 , c, "0.12345 V"); py = py + 35;
    VMin = new LabelClass( px, py, 29 , c, "0.12345 V"); py = py + 35;
    VMax = new LabelClass( px, py, 29 , c, "0.12345 V"); py = py + 35;

    ClearBufferBtn = new ButtonClass( 540, 110 , 220, 50,29,   "Clear Buffer");
    ClearBufferBtn->SetListener(this, (CallbackFn) &StatisticsPanel::OnClearBtn );

    BufferInfoLabel = new LabelClass( 540, 10, 29 , cLegende, "Readings:");
    BufferInfo = new LabelClass( 680, 10, 29 , 0xFFFFFF, "0001"); 

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

    AddControl(ClearBufferBtn );  
    AddControl(BufferInfoLabel ); 
    AddControl(BufferInfo ); 
}

void StatisticsPanel::Render() 
{
    if( Visible == false) return;

    //Serial.println("StatisticsPanel::Render()");

    CValue->SetText(System.FormatCurrent( System.Buffer.GetCurrent()).c_str());
    CPeakToPeak->SetText(System.FormatCurrent( System.Buffer.GetCurrentPeakToPeak()).c_str());
    CMin->SetText(System.FormatCurrent( System.Buffer.GetCurrentMin()).c_str());
    CMax->SetText(System.FormatCurrent( System.Buffer.GetCurrentMax()).c_str());
    CAverage->SetText(System.FormatCurrent( System.Buffer.GetCurrentAverage()).c_str());
    

    VValue->SetText(System.FormatVoltage( System.Buffer.GetVoltage()).c_str());
    VPeakToPeak->SetText(System.FormatVoltage( System.Buffer.GetVoltagePeakToPeak()).c_str());
    VMin->SetText(System.FormatVoltage( System.Buffer.GetVoltageMin()).c_str());
    VMax->SetText(System.FormatVoltage( System.Buffer.GetVoltageMax()).c_str());
    VAverage->SetText(System.FormatVoltage( System.Buffer.GetVoltageAverage()).c_str());

    BufferInfo->SetText(System.FormatBufferCount().c_str());

    ContainerClass::Render();
}


//----------------------------------------------------------------------
// VoltageSource

VoltageSourcePanel::VoltageSourcePanel(int x, int y ,int w ,int h) 
{
    X=x;
    Y=y;
    W=w;
    H=h;
   
    Title = "VOLTAGE";

    pSourceNumberPad = new NumberPad( System.GetVoltageSource(), 100,10, 600 , 460 );

    pLimitNumberPad = new NumberPad( System.GetVoltageSource()->GetLimit(), 100,10, 600 , 460 );

    pVoltageRangePad = new VoltageRangePad(  100,100, 600 , 300 );

    int startX = THEME_XSTART;
    ModeLabel = new LabelClass( startX, 2, 29 , THEME_SECONDARY_COLOR, "SOURCE VOLTAGE");
    ModeLabel->BgColor = THEME_SECONDARY_BGCOLOR;

    MonValue = new LabelClass(startX, 70, 1,  THEME_VOLTAGE_LABEL_COLOR, "000.00012 V"); 

    // Range-Button
    RangeLabel = new LabelClass( startX, 145, 29 , THEME_SECONDARY_COLOR, "Range");
    RangeButton = new ButtonClass( startX+100, 125 , "4V");
    RangeButton->SetListener( this, (CallbackFn) &VoltageSourcePanel::OnRangeClick);

    SourceLabel = new LabelClass( startX + 220 , 145, 29 , THEME_SECONDARY_COLOR, "Source");
    SourceButton = new ButtonClass( startX+ 320, 125 , "+1.2456V" );
    SourceButton->SetListener( this, (CallbackFn) &VoltageSourcePanel::OnSetSourceClick);

    LimitLabel = new LabelClass( startX + 510 , 145, 29 , THEME_SECONDARY_COLOR, "Limit");
    LimitButton = new ButtonClass( startX+ 590, 125 , "+250mA" );
    LimitButton->SetListener( this, (CallbackFn) &VoltageSourcePanel::OnSetLimitClick);

    


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
    if( pLimitNumberPad == nullptr ) return;

    Display.ShowWindow(pLimitNumberPad);
} 

void VoltageSourcePanel::OnSetSourceClick() 
{   
    if( pSourceNumberPad == nullptr ) return;

    Display.ShowWindow(pSourceNumberPad);
} 

void VoltageSourcePanel::OnRangeClick() 
{   
    if( pVoltageRangePad == nullptr ) return;

    Display.ShowWindow(pVoltageRangePad);
}


void VoltageSourcePanel::Render() 
{
    if( Visible == false) return;

    // Gemessen Wert aus den Buffer anzeigen. 
    MonValue->SetText( System.FormatVoltage( System.Buffer.GetVoltage()).c_str()); 

    // Aktuellen Ranges im Button anzeigen
    RangeClass *range =   System.GetVoltageMeasurement()->GetSelectedRange();

    if( range != nullptr)
    {
        if( range->GetRangeType() == Voltage4V ) RangeButton->SetText("4V");
        if( range->GetRangeType() == Voltage30V ) RangeButton->SetText("30V");
    } else 
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


PowerPanel::PowerPanel(int x, int y ,int w ,int h) 
{
    X=x;
    Y=y;
    W=w;
    H=h;
    Title = "POWER";

    int startX = THEME_XSTART;
   ModeLabel = new LabelClass( startX, 2, 29 , THEME_SECONDARY_COLOR, "MEASURE POWER");
   ModeLabel->BgColor = THEME_SECONDARY_BGCOLOR;

    MonValue = new LabelClass(startX, 70, 1,  THEME_POWER_LABEL_COLOR, "000.00012 W"); 

    RangeButton = new ButtonClass( startX, 120 ,   "RANGE");

   AddControl(ModeLabel);
   AddControl(MonValue);
   AddControl(RangeButton);
}

void PowerPanel::Render() 
{
    MonValue->SetText( System.FormatPower( System.Buffer.GetPower()).c_str()); 
    ContainerClass::Render();
}


GraphClass::GraphClass(int x, int y ,int w ,int h) 
{
    X=x;
    Y=y;
    W=w;
    H=h;

    Title = "GRAPH";

}



void GraphClass::DrawMinMax() 
{
    float max = System.Buffer.GetCurrentMax();
    float min = System.Buffer.GetCurrentMin();
    
    float step = (max - min) / 6.0;

    int x = GetX();
    int y = GetY();

    for( int i=0; i<7 ; i++) 
    {
        GD.cmd_text(x, y , 26 ,OPT_CENTERY, String( max,5 ).c_str());
        max = max - step;
        y = y + RowHeight();
    }

} 


void GraphClass::DrawValues() 
{
    
    if( GetX() < 0  || (GetX()+W) > 800) return; 

    float max = System.Buffer.GetCurrentMax();
    float min = System.Buffer.GetCurrentMin();
    
    if( (max-min) == 0.0 ) return;

    float m0 = (180.0 / (max-min)) ;
    float m1 = 700.0 / 200.0;

    int x = GetX()+100;
    int y = GetY();

    BufferDataClass b[200];

    int  c = System.Buffer.GetData(b, 200);

    /* Punkte zeichnen
    x = GetX()+100;
    GD.Begin(POINTS);
    GD.PointSize(2*16);
    GD.ColorRGB( C1);
    GD.ColorA(0xFF);

    for( int i=0; i< c ; i++) 
    {     
        int ypos = y + (m0 * ( b[i].Current - min) );     
        GD.Vertex2f( (x)  , ypos );
        x = x + m1;    
    }
    */


    x = GetX()+100;

    GD.Begin(LINE_STRIP);
    GD.LineWidth(10);
    GD.ColorRGB( C1);


    for( int i=0; i< c ; i++) 
    {
      
        int ypos = y + (m0 * ( b[i].Current - min) );     
        GD.Vertex2f( (x)  , ypos );
        x = x + m1;    
    }



} 



void GraphClass::Render() 
{
    

    if( Visible == false) return;

    Serial.println(" GraphClass::Render()");

    int h = 180;
    int y = GetY();
    int x = GetX() + 100;
    int x1 = W;

    // Gitterlinien zeichnen
    if (GetX() < 0) x1 =  W+GetX();

        GD.Begin(LINES);
        GD.ColorRGB(0xFFFFFF);
        GD.LineWidth( 4);


        for( int r =0; r<7 ; r++ ) 
        {
            GD.Vertex2f( x,y  );
            GD.Vertex2f( x1, y  );
            y = y + RowHeight();                
        }

        y = GetY();

        for( int c = 0; c<11; c++) 
        {

            GD.Vertex2f( x, y);
            GD.Vertex2f( x, (h+y) );
            x = x + 70;               
        }


        DrawMinMax();

        DrawValues();
}


/*----------------------------------------------------------------
    NumberPad
------------------------------------------------------------------*/

void NumberPad::OnKeyAbort(void) { Display.HideWindow();}
void NumberPad::OnKey01(void) { AddToBuffer('1');}
void NumberPad::OnKey02(void) { AddToBuffer('2');}
void NumberPad::OnKey03(void) { AddToBuffer('3');}
void NumberPad::OnKey04(void) { AddToBuffer('4');}
void NumberPad::OnKey05(void) { AddToBuffer('5');}
void NumberPad::OnKey06(void) { AddToBuffer('6');}
void NumberPad::OnKey07(void) { AddToBuffer('7');}
void NumberPad::OnKey08(void) { AddToBuffer('8');}
void NumberPad::OnKey09(void) { AddToBuffer('9');}
void NumberPad::OnKey00(void) { AddToBuffer('0');}

void NumberPad::OnKeyComma(void) { AddDecimalPointToBuffer('.'); }
void NumberPad::OnKeyBackspace(void) { BackspaceToBuffer();}
void NumberPad::OnKeyClear(void) { Clear();}
void NumberPad::OnKeyPlusMinus(void) { TogglePlusMinus();}

void NumberPad::OnKeyUnit(void) { UnitBase = Base; BufferToDisplay(); }
void NumberPad::OnKeyUnitMillis(void) { UnitBase = Millis; BufferToDisplay(); }
void NumberPad::OnKeyUnitMikros(void) { UnitBase = Mikros; BufferToDisplay(); }
 



NumberPad::NumberPad( INumberPad *device, int x, int y, int w, int h) 
{
        if( device == nullptr) Serial.println("DEVICE IS NULL");

        

    	X=x;
        Y=y;
        W=w;
        H=h;
        pDevice = device; 



        int btnH = 70;  // Button Height
        int btnW = 90;  // Button Width

        int row = 130;  // y-Start
        int col = 30;   // x-Start
        int d= 6;       // Abstand
              
        InputField = new LabelClass( 50, 60 ,  2,  C1, "");
        
        Btn1 = new ButtonClass( col, row , btnW, btnH, 30, "1"); 
        col = col + btnW +d;
        Btn2 = new ButtonClass( col, row ,  btnW, btnH, 30,  "2");
        col = col + btnW +d;
        Btn3 = new ButtonClass( col, row ,  btnW, btnH, 30, "3");
        col = col + btnW +d;
    
        BtnUnit = new ButtonClass( col, row ,  btnW, btnH,30, (pDevice->GetSourceType() == VoltageSourceType) ? "V" : "A" );

        col = col + btnW +d;
        BtnAbort = new ButtonClass( col, row , 140, btnH, 30, "Abort");        
        
        row = row + btnH + d;
        col = 30;

        Btn4 = new ButtonClass( col, row , btnW, btnH,  30,"4");
        col = col + btnW +d;
        Btn5 = new ButtonClass( col, row ,  btnW, btnH, 30,  "5");
        col = col + btnW +d;
        Btn6 = new ButtonClass( col, row ,  btnW, btnH, 30,"6");
        col = col + btnW +d;
        BtnUnitMilli = new ButtonClass( col, row ,  btnW, btnH,30, (pDevice->GetSourceType() == VoltageSourceType) ? "mV" : "mA" );
        col = col + btnW +d;
        BtnClear = new ButtonClass( col, row , 140, btnH, 30, "Clear");
       
        row = row + btnH + d;
        col = 30;

        Btn7 = new ButtonClass( col, row , btnW, btnH, 30, "7");
        col = col + btnW +d;
        Btn8 = new ButtonClass( col, row ,  btnW, btnH, 30,  "8");
        col = col + btnW +d;
        Btn9 = new ButtonClass( col, row ,  btnW, btnH, 30, "9");
        col = col + btnW +d;
        BtnUnitMicro = new ButtonClass( col, row , btnW, btnH, 30,(pDevice->GetSourceType() == VoltageSourceType) ? "uV" : "uA");
        col = col + btnW +d;
        BtnBackspace = new ButtonClass( col, row , 140, btnH, 30, "Back");

        row = row + btnH + d;
        col = 30;


        Btn0 = new ButtonClass( col, row , btnW, btnH, 30, "0");
        col = col + btnW +d;
        BtnPlusMinus = new ButtonClass( col, row , btnW, btnH, 30,  "+/-");
        col = col + btnW +d;
        BtnDecimal = new ButtonClass( col, row ,  btnW, btnH, 30, ".");
        col = col + btnW +d;
        BtnNone = new ButtonClass( col, row ,  btnW, btnH, 30, " ");
        col = col + btnW +d;
        BtnEnter = new ButtonClass( col, row , 140, btnH, 30, "Enter");

        Clear(); // Buffer
        AddListner();
        AddControls();
       
}



void NumberPad::Show() 
{


    float v = pDevice->GetValue();



    if( pDevice != nullptr) 
    {
        SetValue( v);
    }
    
}


// Übergabe erfolgt in der Base-Auflösung
void NumberPad::SetValue( float v) 
{


    switch( UnitBase ) 
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
    InputBuffer.concat( v );
    BufferToDisplay();

    /*
        char buf[20];
        char* string = dtostrf(num, 4, 2, buf);
        return concat(string, strlen(string));
    */

     
} 



void NumberPad::AddListner() 
{
    
    Btn0->SetListener(this, (CallbackFn) &NumberPad::OnKey00 );
    Btn1->SetListener(this, (CallbackFn) &NumberPad::OnKey01 );
    Btn2->SetListener(this, (CallbackFn) &NumberPad::OnKey02 );
    Btn3->SetListener(this, (CallbackFn) &NumberPad::OnKey03 );
    Btn4->SetListener(this, (CallbackFn) &NumberPad::OnKey04 );
    Btn5->SetListener(this, (CallbackFn) &NumberPad::OnKey05 );
    Btn6->SetListener(this, (CallbackFn) &NumberPad::OnKey06 );
    Btn7->SetListener(this, (CallbackFn) &NumberPad::OnKey07 );
    Btn8->SetListener(this, (CallbackFn) &NumberPad::OnKey08 );
    Btn9->SetListener(this, (CallbackFn) &NumberPad::OnKey09 );

     BtnEnter->SetListener( this, (CallbackFn) &NumberPad::OnKeyEnter );
    
     BtnAbort->SetListener( this, (CallbackFn) &NumberPad::OnKeyAbort );

    BtnDecimal->SetListener( this, (CallbackFn) &NumberPad::OnKeyComma );
    BtnBackspace->SetListener( this, (CallbackFn) &NumberPad::OnKeyBackspace );
    BtnClear->SetListener( this, (CallbackFn) &NumberPad::OnKeyClear );
    BtnPlusMinus->SetListener( this, (CallbackFn) &NumberPad::OnKeyPlusMinus );

    BtnUnit->SetListener( this, (CallbackFn) &NumberPad::OnKeyUnit );
    BtnUnitMilli->SetListener( this, (CallbackFn) &NumberPad::OnKeyUnitMillis );
    BtnUnitMicro->SetListener( this, (CallbackFn) &NumberPad::OnKeyUnitMikros );
    

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
   
   
    if( pDevice != nullptr) 
    {
        pDevice->SetValue( GetAsFloat() );
        Display.HideWindow();
    } else 
    {
        Serial.println("[Error] NumberPad kat keinen Device");
    }
    
}


void NumberPad::ValidateInput() 
{
    
    float v = GetAsFloat();

    if( pDevice->ValidateValue(v) == false ) 
    {
        
        InputField->SetFontColor(THEME_ERROR_COLOR);
    } else 
    {
        InputField->SetFontColor(C1);
    }
    
    
}




// Eingabefeld im Display aktualisieren
void NumberPad::BufferToDisplay() 
{   
    ValidateInput();
    
    DisplayBuffer = "";
    DisplayBuffer.concat( InputBuffer.c_str());
    if( UnitBase == Base) DisplayBuffer.concat( (pDevice->GetSourceType() == VoltageSourceType) ? " V" : " A");
    if( UnitBase == Millis) DisplayBuffer.concat( (pDevice->GetSourceType() == VoltageSourceType) ? " mV" : " mA");
    if( UnitBase == Mikros) DisplayBuffer.concat( (pDevice->GetSourceType() == VoltageSourceType) ? " uV" : " uA");

    InputField->SetText( DisplayBuffer.c_str());
}


void NumberPad::AddDecimalPointToBuffer(char c)
{
	if (InputBuffer.indexOf(c) == -1) {
		InputBuffer.concat(c);
		BufferToDisplay();
	}
}


void NumberPad::TogglePlusMinus() 
{
    int idx = -1;
    if( (idx = InputBuffer.indexOf("+") ) != -1 )
    {
        InputBuffer.replace("+","-");        
    } else 
    {
        InputBuffer.replace("-","+");        
    }  
    BufferToDisplay();  
} 


void NumberPad::AddToBuffer( char c ) 
{

    if (InputBuffer.length() < MAX_INPUT_LENGTH) {
		InputBuffer.concat(c);
		BufferToDisplay();  
	}
}




void NumberPad::BackspaceToBuffer(void)
{
    if (InputBuffer.length() > 1) {
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
	
    switch( UnitBase) 
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
   
    GD.LineWidth(16*2);
    GD.Vertex2f( X, Y );
    GD.Vertex2f( (X+W), (Y+H));


    GD.ColorRGB(0x1A3857);

    GD.Vertex2f( (X+2), (Y+2));
    GD.Vertex2f( (X+W-2), (Y+H-2));


    // Render Input-Field
    GD.Begin(RECTS);
    GD.ColorRGB(0x808080);
    

    GD.LineWidth(16*2);
    GD.Vertex2f( X+30, Y+20 );
    GD.Vertex2f( (X+530), (Y+100));
    GD.ColorRGB(0x000000);


    GD.LineWidth(16*2);
    GD.Vertex2f( X+30+2, Y+20+2 );
    GD.Vertex2f( (X+530-2), (Y+100-2));


    

    ContainerClass::Render();

}