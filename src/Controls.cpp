
#include "Controls.h"

#include "Beeper.h"





void SwipeControllerClass::Add( ContainerClass *p) 
{
    if( p != nullptr) 
    {
        if( Counter < MAX_SWIPE_PANELS ) 
        {
            Panels[Counter] = p;
            p->Visible = false;
            Counter = Counter +1;
        }
    }
}

/*
    Liefert den nächsten Index in der Liste, ohne den aktuellen Index
    zu verändert.
*/
uint8_t SwipeControllerClass::Next() 
{
    int idx = Index +1; 

    return ( idx < Counter) ? idx : 0;

}

/*
    Liefert den vorherigen Index in der Liste, ohne den aktuellen Index
    zu verändert.
*/
uint8_t SwipeControllerClass::Previous() 
{
    int idx = Index -1; 
    return ( idx >= 0) ? idx : Counter-1;

 
}

void SwipeControllerClass::SetCurrent( uint8_t idx) 
{   
    if( idx < Counter)  
    { 
        Index = idx;
        if( Panels[Index] != nullptr) 
        {
            Panels[Index]->Visible = true;           
        }
    }
}


#define SWIPE_NONE 0
#define SWIPE_LEFT 1
#define SWIPE_RIGHT 2

void SwipeControllerClass::TouchStart( int x, int y) 
{

    ContainerClass *n = GetPanelByIndex(Next());    
    ContainerClass *p = GetPanelByIndex(Previous());    

    p->X = - 800; 
    p->Visible = true;
    n->X = + 800;
    n->Visible = true;

    Direction = 0; 
} 

void SwipeControllerClass::TouchEnd( int x, int y) 
{
  
    if (Direction == SWIPE_NONE) return; 
    

    ContainerClass *n = GetPanelByIndex(Next());    
    ContainerClass *p = GetPanelByIndex(Previous());    
    ContainerClass *c = GetPanelByIndex(Current()); 

    p->Visible = false;
    c->Visible = false;
    n->Visible = false;

    if( Direction == SWIPE_LEFT ) 
    {
        n->X=0; n->Visible = true;
        SetCurrent( Next() );
    }

    if( Direction == SWIPE_RIGHT ) 
    {
        p->X=0; p->Visible = true;
        SetCurrent(  Previous() );
    }
    
    Direction = SWIPE_NONE;
} 


void SwipeControllerClass::TouchSwipe( int x, int y, int dx , int dy) 
{


    ContainerClass *n = GetPanelByIndex(Next());    
    ContainerClass *p = GetPanelByIndex(Previous());    
    ContainerClass *c = GetPanelByIndex(Current()); 

    Direction =  ( dx > 0 ) ? SWIPE_LEFT: SWIPE_RIGHT; 

    if( Direction == SWIPE_RIGHT) 
    {   
        c->X = (dx *-1 );
        p->X = -800 + (dx * -1);
    }

    if( Direction == SWIPE_LEFT ) 
    {
        c->X = (dx *-1);
        n->X = 800 - (dx );
    }
    

}



/*
    Prüft, ob der Punkt in den Control liegt
*/
bool ControlClass::IsIn( int x, int y)
{    
    x = x+40; // TODO Klären warum Offset notwendig ist
    bool in =  ( (x >= ( GetX()) ) && (y >= GetY()) && (x<=(W+GetX())) && (y<=(H+GetY())) );
         
    return in;
}

int ControlClass::GetY()
{
    ControlClass *p = Parent;
    int y = Y;

    while( p != nullptr ) 
    {
        y = y + p->Y;
        p = p->Parent;
    }
    return y;
}


int ControlClass::GetX()
{
    ControlClass *p = Parent;
    int x = X;

    while( p != nullptr ) 
    {
        x = x + p->X;
        p = p->Parent;
    }
    return x;
}


ControlClass* ControlClass::FindFocusControl( int x, int y) 
{
    ControlClass* result = nullptr;

    if( IsDisabled == true ) return nullptr;

    result = (IsFocusable == true && (IsIn( x,y)==true)) ? this : nullptr; 
 
    
    return result;
} 

void ControlClass::DispatchTouchEvent(  uint8_t eventtype, int x, int y, int dx, int dy) 
{
    
    if( eventtype == TOUCH_START) TouchStart(x,y);
    if( eventtype == TOUCH_END) TouchEnd(x,y);
    if( eventtype == TOUCH_SWIPE) TouchSwipe(x,y, dx,dy);
    
}


//------------------------------------------------------------
// BUTTON
//------------------------------------------------------------

ButtonClass::ButtonClass( int x, int y, const char *text ) 
{
    
    FontSize = THEME_BUTTON_FONT_SIZE;
    BgColor = BUTTON_BGCOLOR;
    FontColor = BUTTON_FONT_COLOR;
    IsFocusable=true;    
    Text = text;

    X = x;
    Y = y;
    W =  (strlen(text) * (FontSize-10) ) + 10;
    H  = FontSize + 20;

    if( strlen(text) <= 3) W = 100;
    

   

}

ButtonClass::ButtonClass( int x, int y, int w, int h, int fontSize, const char *text ) 
{
    
    FontSize = fontSize;
    BgColor = BUTTON_BGCOLOR;
    FontColor = BUTTON_FONT_COLOR;
    IsFocusable=true;    
    Text = text;

    X = x;
    Y = y;
    W = w;
    H  =h;


   

}


void ButtonClass::Render()  
{
    
    if (Visible == false) return;
    
    FontColor = (IsDisabled == true ) ? BUTTON_FONT_COLOR_DISABLED: BUTTON_FONT_COLOR;
    BgColor = (IsDisabled == true ) ? BUTTON_BGCOLOR_DISABLED: BUTTON_BGCOLOR;


    GD.Tag(99);
    GD.cmd_fgcolor(BgColor); // Button Color
    GD.ColorRGB(FontColor);  // Text Color
    GD.cmd_button(  GetX(),  GetY(), W, H, FontSize, OPT_CENTER, Text.c_str() );

}

void ButtonClass::TouchStart(int x, int y) 
{   
    if( IsDisabled == true) return;

    // TODO Dieser Event kann entfallen 
    if( pCallback != nullptr && pCallbackObject != nullptr) 
    {
        (pCallbackObject->*pCallback)();
    }

    if( pBtnClickFn != nullptr && pCallbackObject != nullptr)
    {
        (pCallbackObject->*pBtnClickFn)(this);
    }

    Beeper.KeyPress();

} 

void ButtonClass::TouchEnd(int x, int y) 
{   
    if( IsDisabled == true) return;   
} 

//------------------------------------------------------------
// LABEL
//------------------------------------------------------------


LabelClass::LabelClass( int x, int y, uint16_t fontSize, uint32_t fontColor, const char *text ) 
{
    X = x;
    Y = y;
    FontSize = fontSize;
    FontColor = fontColor;
    BgColor = THEME_BACKGROUND_COLOR; // ??
    Text = text;


}


void LabelClass::Render()  
{
    if( Visible == false) return;

    GD.ColorRGB(FontColor);
    GD.cmd_text(GetX(), GetY() , FontSize , OPT_CENTERY, Text.c_str());
    
};


//---------------------------------------------------------------
// ContainerClass
//---------------------------------------------------------------



void ContainerClass::AddControl( ControlClass *p) 
{
    if( (ControlCounter < MAX_CONTROLS) && (p != nullptr) ) 
    {
        pControls[ControlCounter] = p;
        p->Parent = this;
        ControlCounter = ControlCounter+1;
    }
}



void ContainerClass::Render( ) 
{
    
   if( Visible == false) return;


    for( int i=0; i<ControlCounter; i++ ) 
    {
        ControlClass *p = pControls[i];
        if( p != nullptr) {
           
             

            p->Render(); 
        }
    }


}


void ContainerClass::DispatchTouchEvent( uint8_t eventtype, int x, int y, int dx, int dy) 
{
   if( Visible == false) return;

    
    for( int i=0; i<ControlCounter; i++ ) 
    {
        ControlClass *p = pControls[i];
        if( p != nullptr) {  
            bool in = p->IsIn(x,y);
            
            if( in )  p->DispatchTouchEvent(eventtype, x ,y, dx ,dy );
        }
    }
    
    if( eventtype == TOUCH_START) TouchStart(x,y);
    if( eventtype == TOUCH_END) TouchEnd(x,y);
    if( eventtype == TOUCH_SWIPE) TouchSwipe(x,y, dx, dy);
    
}


/*
    Sucht ein Control, welches einen Focus erhalten kann
*/
ControlClass* ContainerClass::FindFocusControl( int x, int y ) 
{
    ControlClass* found = nullptr;
    
    for( int i=0; i< ControlCounter; i++ ) 
    {
        ControlClass *p = pControls[i];
        if( p != nullptr) 
        {
            found = p->FindFocusControl(x,y);
            if( found != nullptr) break;
        }

    }
    Serial.println("FindFocusControl " + (found != nullptr ) );

    return found;
}



void  ContainerClass::TRACE( ContainerClass *p, uint level) 
{
    
    Serial.print("Control Count "); Serial.println(ControlCounter);
    Serial.print("Control Level "); Serial.println(level);
    
    for(int i=0; i<= ControlCounter-1;i++)     
    {
        if( pControls[i]->GetType() == ContainerType ) 
        {
            TRACE((ContainerClass*) pControls[i], ++level );
        }
    }

}
