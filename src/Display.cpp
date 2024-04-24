
#include "display.h"

#include "Beeper.h"

DisplayClass::DisplayClass()
{
    X = 0;
    Y = 0;
    W = 800;
    H = 480;
    Input.TouchState = TOUCH_END;

    pPrimary = new PrimaryPanel(0, 0, 800, 240);

    pSecondary = new SecondaryPanel(0, 240, 800, 240);

    AddControl(pPrimary);
    AddControl(pSecondary);

    RegisterSubscriber( pSecondary);

    Beeper.KeyPress();
}

void DisplayClass::ShowWindow(WindowClass *p)
{
    pModalWindow = p;

    if (pModalWindow != nullptr)
    {
        HideFocus();
        pModalWindow->Show();
    }


}

void DisplayClass::Render()
{

    ContainerClass::Render();

    if (pModalWindow != nullptr)
    {
        pModalWindow->Render();
    }

    RenderFocus();
}

void DisplayClass::RenderFocus()
{

    uint32_t c = 0xf2ea0a;
    if (pFocus != nullptr)
    {

        int x = (pFocus->GetX() + 1);
        int x1 = (pFocus->GetX() + pFocus->W - 1);
        int y = (pFocus->GetY() + 1);
        int y1 = ((pFocus->GetY() + +pFocus->H - 1));

        GD.Begin(LINE_STRIP);
        GD.LineWidth(1 * 16);
        GD.ColorRGB(c);
        GD.Vertex2f(x, y);
        GD.Vertex2f(x1, y);
        GD.Vertex2f(x1, y1);
        GD.Vertex2f(x, y1);
        GD.Vertex2f(x, y);
    }
}

/**
 * @brief TouchStat-Event erzeugt für einen Button MouseDown und für 
 * einen Subscriber einen TouchStart-Event.
 * 
 * @param x 
 * @param y 
 */
void DisplayClass::OnTouchStart(int x, int y)
{
    ControlClass *c = nullptr;

   

    c = (pModalWindow != nullptr) ?  pModalWindow->FindFocusControl(x, y) : FindFocusControl(x,y);    

    // MouseDown-Event
    if (c != nullptr && Input.pTouchObj == nullptr)
    {        
       
        SetFocus(c);
        Input.pTouchObj = c;  

        c->TouchStart(  x, y );     
        Input.TouchState = TOUCH_START; 
    }

    // TouchStart für einen Subscriber
    if( c==nullptr && pModalWindow == nullptr && Input.pTouchObj == nullptr) 
    {
        if( pSubscriber != nullptr) {
          
            pSubscriber->TouchStart(x,y);
            Input.TouchState = TOUCH_SWIPE;
        }
    }


    return;


}
void DisplayClass::OnTouchEnd(int x, int y)
{
  
    
    if( Input.TouchState == TOUCH_END) return;

    // MouseUp-Event
    if( Input.pTouchObj != nullptr ) 
    {
        
        Input.pTouchObj->TouchEnd(x,y);
        Input.pTouchObj = nullptr;
    } else 
    {
         // TouchEnd für einen Subscriber
        if( pSubscriber != nullptr){
           
             pSubscriber->TouchEnd(x,y);
        }
    }
    Input.TouchState = TOUCH_END;
   
}
void DisplayClass::OnTouchSwipe(int x, int y, int dx, int dy)
{

    if( Input.pTouchObj == nullptr) {
        pFocus = nullptr;
       
        if( pSubscriber != nullptr) pSubscriber->TouchSwipe(x,y, dx, dy);
    }
}

void DisplayClass::ReadInput()
{
    GD.get_inputs();

    if (GD.inputs.tag)
    {
        // Touch Kalibrierung
        GD.inputs.tag_x = GD.inputs.tag_x - 40;

        
        if( Input.TouchState == TOUCH_END ) 
        {
            Input.StartX = GD.inputs.tag_x; //Start Position merken
            Input.StartY = GD.inputs.tag_y;
            OnTouchStart(GD.inputs.tag_x, GD.inputs.tag_y );
           
            
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

        OnTouchEnd(Input.StartX , Input.StartY  );           

       
    }
}

//********************************************************************
//	GUI
//********************************************************************

HeaderPanel::HeaderPanel(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;

    int startX = 20;
    RemoteLabel = new LabelClass(startX, 16, 29, C0, "Local");
    SystemInfo = new LabelClass(800 - 200, 16, 29, C0, "A-SMU V0.1");

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
    GD.Vertex2f(0, ROW);
    GD.Vertex2f(800, ROW);

    ContainerClass::Render();
}

/*
    PrimaryPanel wir zur Anzeige der Messungen verwendet.

    TODO:
    Umbau für die Messungen
    Funktion Button
    FunktionsPad

*/

PrimaryPanel::PrimaryPanel(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;

    Header = new HeaderPanel(0, 0, 800, 32);

    
    CurrentMeasure = new CurrentMeasurmentPanel(0, 50, 800, 240 - 42);
    VoltageMeasure = new VoltageMeasurmentPanel(0, 50, 800, 240 - 42);
    ResistorMeasure = new ResistorMeasurmentPanel(0, 50, 800, 240 - 42);
    PowerMeasure = new PowerMeasurmentPanel(0, 50, 800, 240 - 42);

    AddControl(Header);
    AddControl(CurrentMeasure);
    AddControl(VoltageMeasure);
    AddControl(ResistorMeasure);
    AddControl(PowerMeasure);

}

void PrimaryPanel::Render()
{


    
    MeasurementClass *m = System.GetSelectedMeasurement();
    if( m == nullptr) return;



    CurrentMeasure->Visible = (m->Type == CurrentType) ? true : false;
    VoltageMeasure->Visible = (m->Type == VoltageType) ? true : false;
    ResistorMeasure->Visible = (m->Type == ResistorType) ? true : false;
    PowerMeasure->Visible = (m->Type == PowerType) ? true : false;

    ContainerClass::Render();
    

}

//--------------------------------------------
// Secondary Panel
//--------------------------------------------

SecondaryPanel::SecondaryPanel(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;

    SwipeController = new SwipeControllerClass();

    TitleLabel = new LabelClass(THEME_XSTART, 16, 29, THEME_SECONDARY_COLOR, "Title");
    TitleLabel->BgColor = THEME_SECONDARY_BGCOLOR;



    Graph = new GraphPanelClass(0, 50, 800, 240 - 42);
    Statistics = new StatisticsPanel(0, 50, 800, 240 - 42);


    // Source
    VoltageSource = new VoltageSourcePanel( 0, 50 , 800, 240 - 42);
    



    SwipeController->Add(VoltageSource);
    SwipeController->Add(Statistics);
    SwipeController->Add(Graph);

    AddControl(TitleLabel);

    AddControl(VoltageSource);
    AddControl( Graph);
    AddControl(Statistics);

    SwipeController->SetCurrent(0);
}

void SecondaryPanel::TouchSwipe(int x, int y, int dx, int dy)
{

    SwipeController->TouchSwipe(x, y, dx, dy);
}

void SecondaryPanel::Render()
{

    // HEADER
    //----------------------------------------------

    // Render Background
    GD.Begin(RECTS);
    GD.LineWidth(16 * 2);
    GD.ColorRGB(THEME_SECONDARY_BGCOLOR);

    GD.Vertex2f(GetX(), GetY());
    GD.Vertex2f((GetX() + W), (GetY() + H));

    int ROW_HEIGHT = 32;

    // First Row
    GD.LineWidth(16 * 1);
    GD.Begin(LINE_STRIP);
    GD.ColorRGB(THEME_SECONDARY_COLOR);

    GD.Vertex2f(GetX(), (GetY() + ROW_HEIGHT));
    GD.Vertex2f(800, (GetY() + ROW_HEIGHT));

    // Render Tabs
    int count = SwipeController->GetCount();
    int distance = 30; // in px
    int x = 400 - (distance * count / 2);

    SetHeaderTitle();

    for (int i = 0; i < count; i++)
    {

        DrawBall(x + (i * distance), GetY() + 16, SwipeController->Current() == i);
    }

    //-----------------------------

    ContainerClass::Render();
}

void SecondaryPanel::SetHeaderTitle()
{
    TitleLabel->SetText("Secondary");
    int cpIdx = SwipeController->Current();
    ContainerClass *p = SwipeController->GetPanelByIndex(cpIdx);
    if (p != nullptr)
        TitleLabel->SetText(p->Title);
}

void SecondaryPanel::DrawBall(int x, int y, bool set)
{
    GD.Begin(POINTS);
    GD.PointSize(16 * 6);
    if (set == false)
    {
        GD.ColorRGB(255, 255, 255);
    }
    else
    {
        GD.ColorRGB(0, 0, 0);
    }
    GD.Vertex2ii(x, y);
}


