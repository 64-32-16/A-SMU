

#ifndef _Controls_h
#define _Controls_h

#include "arduino.h"
#include <GD2.h>
#include "System.h"


#define MAX_CONTROLS 1024

#define TOUCH_NONE 0
#define TOUCH_START 1
#define TOUCH_END 2
#define TOUCH_SWIPE 3


#define BTN_SMALL 0;
#define BTN_MIDDLE = 1;
#define BTN_BIG = 2;


#define C0 0xFFFFFF // White
#define C1 0x5bed59 // CurrentColor

#define THEME_BUTTON_FONT_SIZE 29

// Okay Green
#define THEME_CURRENT_LABEL_COLOR 0xf000
#define THEME_VOLTAGE_LABEL_COLOR 0x5cacf7

#define BUTTON_FONT_COLOR 0x000000
#define BUTTON_FONT_COLOR_DISABLED 0x808080

#define BUTTON_BGCOLOR 0x808080
#define BUTTON_BGCOLOR_DISABLED 0x707070

#define THEME_FOCUS_COLOR 0xf2ea0a;






//------------------------------------

#define THEME_PRIMARAY_COLOR 0x5bed59



//#define THEME_SECONDARY_COLOR 0x305bb8

#define THEME_SECONDARY_COLOR 0x5cacf7
#define THEME_SECONDARY_BGCOLOR 0x101010

#define THEME_POWER_LABEL_COLOR 0xf2ea0a


#define THEME_ERROR_COLOR 0xff000E

// Klären
#define THEME_BUTTON_SELECTED_BGCOLOR 0x94b2
#define THEME_BUTTON_HEIGHT 48

#define THEME_BACKGROUND_COLOR 0x0000

#define THEME_CURRENT_LABEL_COLOR 0xf000




#define THEME_XSTART 20
#define MAX_SWIPE_PANELS 10




class ContainerClass;
class ControlClass;
class ButtonClass;



typedef  void (ControlClass::*CallbackFn)( void); 
typedef  void (ControlClass::*OnBtnClickFn)( ButtonClass *p);








// Abstrakte Control Klasse
class ControlClass 
{

 public:
	 int X=0; 
	 int Y=0;
	 int W=100;
	 int H=32;	

	 uint32_t BgColor = 0x000000;
	 
	 bool IsFocusable=false; 
	 //bool IsSelected=false;
	 bool Visible = true;
	 bool IsDisabled = false;

	 ControlClass *Parent = nullptr;  // TODO ACHTUNG SOLLTE CONTAINER SEIN

	 virtual const char* Classname() {return "Control";}

	 virtual void Render() {};
	 

	 virtual ControlClass* FindFocusControl( int x, int y); 

	 virtual void TouchStart( int x, int y) { };
	 virtual void TouchEnd(int x, int y) {};
	 virtual void TouchSwipe(int x, int y, int dx, int dy) {};
	 
	 virtual void DispatchTouchEvent( uint8_t eventtype, int x, int y,  int dx, int dy); 
	 bool IsIn( int x, int y);

	 int GetX(); 
	 int GetY(); 

protected:




};

// Ein Container mit (n) Controls
class ContainerClass: public ControlClass 
{
	public:
	    void AddControl( ControlClass *p);
		const char* Title = {0};

		void Render() override;
	    const char* Classname() override {return "Container";}
		ControlClass *pControls[MAX_CONTROLS];
		void DispatchTouchEvent( uint8_t eventtype, int x, int y, int dx, int dy) override; 

		bool IsActive() {return (GetX() < 0  || (GetX()+W) > 800  ) ;}

		ControlClass* FindFocusControl( int x, int y) override;

	protected:
		
		
		

		
		uint16_t ControlCounter = 0; 

};







class ButtonClass: public ControlClass 
{


 public:
	 ButtonClass( int x, int y, const char *text );
	 ButtonClass( int x, int y, int w, int h, int fontSize, const char *text );
	 ButtonClass() {IsFocusable=true;}
	 
	
	 const char* Classname() override {return "Button";}

	 void SetText(const char* text) { Text = "";  Text.concat(text); }
	 String GetText() { return Text;}
	 void TouchStart( int x, int y) override ;
	 void TouchEnd(int x, int y) override;


	 void Render()  override;
	 
	 void SetListener ( ControlClass *pObj, CallbackFn p) { pCallbackObject = pObj;  pCallback = p; }
	 void SetClickEvent ( ControlClass *pObj, OnBtnClickFn p) { pCallbackObject = pObj;  pBtnClickFn = p; }

 protected:
     uint32_t FontColor = BUTTON_FONT_COLOR; 
	 uint32_t FontSize = THEME_BUTTON_FONT_SIZE;
	 String Text;

	 CallbackFn pCallback = nullptr;  // Methoden-Zeiger
	 ControlClass *pCallbackObject = nullptr; // Methoden-Objekt 

	 
	 OnBtnClickFn pBtnClickFn = nullptr;  // Button Click

	

};


class LabelClass: public ControlClass 
{


 public:

	 
	 LabelClass( int x, int y, uint16_t fontSize,  uint32_t fontColor, const char *text );
	 const char* Classname() override {return "Label";}

	 void SetText( const char* text ) {Text = "";  Text.concat(text);}
	 void Render()  override;
	 LabelClass() {IsFocusable=false;}
	 void SetFontColor( uint32_t color ) { FontColor = color; }

 protected:
	 String Text;

	 uint32_t FontColor = THEME_PRIMARAY_COLOR; 
	 uint32_t FontSize =  THEME_BUTTON_FONT_SIZE;

};


class WindowClass: public ContainerClass 
{
	public:
		const char* Classname() override {return "Window";}
		void virtual Show() {}
	protected:

};


class SwipeControllerClass 
{
	public:


		void Add( ContainerClass *p);
		uint8_t Next();
		uint8_t Previous();
		uint8_t Current() {return Index;};
		void SetCurrent(uint8_t idx);
	

		ContainerClass *GetPanelByIndex( uint8_t idx) { return Panels[idx]; }
		void TouchSwipe( int x, int y, int dx , int dy);
		void TouchEnd(int x, int y );
		void TouchStart(int x, int y );

		int GetCount() { return Counter; }

	protected:
		int Counter = 0;
		int Index = 0;
		ContainerClass *Panels[MAX_SWIPE_PANELS];
		int Direction = 0;

		
};


#endif
