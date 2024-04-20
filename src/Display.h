

#ifndef _Display_h
#define _Display_h

#include "arduino.h"
#include <GD2.h>
#include "Controls.h"
#include "System.h"
#include "Display/MeasurmentPanel.h"




class InputClass 
{
	public:
		int StartX;   // Press und Release Position
		int StartY;   // Press und Release Position 

		int TrackX;
		int TrackY;
	
		uint8_t TouchState = 0; // 0=Kein Input // 1=Pressed // 2=Released

};




//********************************************************************
//	GUI 
//********************************************************************


#define MAX_INPUT_LENGTH 6

enum UnitBaseEnum 
{
	Base,
	Millis,
	Mikros

};




//-----------------------------------------------------------------------



class NumberPad: public WindowClass
{
	public:
		LabelClass *InputField;
		ButtonClass *Btn0;
		ButtonClass *Btn1;
		ButtonClass *Btn2;
		ButtonClass *Btn3;
		ButtonClass *Btn4;
		ButtonClass *Btn5;
		ButtonClass *Btn6;
		ButtonClass *Btn7;
		ButtonClass *Btn8;
		ButtonClass *Btn9;
		ButtonClass *BtnDecimal;
		ButtonClass *BtnUnit;
		ButtonClass *BtnNone;
		
		ButtonClass *BtnUnitMilli;
		ButtonClass *BtnUnitMicro;
		ButtonClass *BtnPlusMinus;
		ButtonClass *BtnAbort;
		ButtonClass *BtnEnter;
		ButtonClass *BtnClear;
		ButtonClass *BtnBackspace;

		UnitBaseEnum UnitBase = Base;

		NumberPad( INumberPad *device, int x, int y, int w, int h);

		const char* Classname() override {return "NumberPad";}

		void Render()  override;

		void Show() override;

	protected:

		void OnKey01();
        void OnKey02();
        void OnKey03();
        void OnKey04();
        void OnKey05();
        void OnKey06();
        void OnKey07();
        void OnKey08();
        void OnKey09();
        void OnKey00();
        void OnKeyEnter();
        void OnKeyESC();
        void OnKeyComma();
        void OnKeyBackspace();
		void OnKeyAbort();
		void OnKeyClear();
		void OnKeyPlusMinus();
		void OnKeyUnit();
		void OnKeyUnitMillis();
		void OnKeyUnitMikros();


		String InputBuffer;
		String DisplayBuffer;
        void AddToBuffer(char c);
        void BufferToDisplay();
        void AddDecimalPointToBuffer(char c);
        void BackspaceToBuffer(void);
		float GetAsFloat(void);
		void TogglePlusMinus();

		void AddListner(void);
		void AddControls();
		void Clear();
		void SetValue( float v); 

		INumberPad *pDevice;	// 

		void ValidateInput();   // Aktuelle Eingabe mit dem Device vergleichen.


};

class HeaderPanel: public ContainerClass 
{
	public:
		HeaderPanel( int x, int y,  int w, int h);

		void Render()  override;
		const char* Classname() override {return "HeaderPanel";}

		LabelClass *RemoteLabel;
		LabelClass *SystemInfo;

		LabelClass *BufferLabel;
	
	protected: 
	
};


class PrimaryPanel: public ContainerClass 
{
	public:
		PrimaryPanel( int x, int y,  int w, int h);

		void Render()  override;
		const char* Classname() override {return "PrimaryPanel";}

		HeaderPanel *Header;

		LabelClass *RemoteLabel;
		LabelClass *WiFiLabel;
		LabelClass *SystemInfo;



		LabelClass *CurrentMONLabel; 
		LabelClass *BufferLabel;
		LabelClass *ModeLabel; // ====> TITLE



		LabelClass *RangeLabel;
		ButtonClass *RangeButton;

		VoltageRangePad *pVoltageRangePad;
		//CurrentRangePad *pCurrentRangePad;
	
	protected: 
		void OnRangeClick();
	
};

class PowerPanel: public ContainerClass 
{
	public:
		PowerPanel( int x, int y,  int w, int h);
		const char* Classname() override {return "PowerPanel";}


		LabelClass *MonValue; 

		void Render() override;

	protected: 
		LabelClass *ModeLabel;
		ButtonClass *RangeButton;
		

};


class StatisticsPanel: public ContainerClass 
{
	public:
		StatisticsPanel( int x, int y,  int w, int h);
		const char* Classname() override {return "StatisticsPanel";}


		LabelClass *PeakToPeak;
		LabelClass *Value;
		LabelClass *Average;
		LabelClass *Min;
		LabelClass *Max;
		
		LabelClass *CPeakToPeak;
		LabelClass *CValue;
		LabelClass *CAverage;
		LabelClass *CMin;
		LabelClass *CMax;		

		LabelClass *VPeakToPeak;
		LabelClass *VValue;
		LabelClass *VAverage;
		LabelClass *VMin;
		LabelClass *VMax;	
		 
		LabelClass *BufferInfoLabel;	
		LabelClass *BufferInfo;	


		ButtonClass *ClearBufferBtn;

		void Render() override;

		void OnClearBtn();

	protected: 

	
};


class GraphClass: public ContainerClass 
{
	public:
		GraphClass( int x, int y,  int w, int h);
		const char* Classname() override {return "GraphClass";}
		void Render()  override;

	

		int RowHeight() { return 30;}
	protected: 

		void DrawMinMax(); 
		void DrawValues();

};


class VoltageSourcePanel: public ContainerClass 
{
	public:
		VoltageSourcePanel( int x, int y,  int w, int h);
		const char* Classname() override {return "VoltageSourcePanel";}
		void Render()  override;

	protected: 
		LabelClass *ModeLabel;
		LabelClass *MonValue; 

		ButtonClass *RangeButton;
		LabelClass *RangeLabel;

		ButtonClass *SourceButton;
		LabelClass *SourceLabel;

		ButtonClass *LimitButton;
		LabelClass *LimitLabel;

		NumberPad *pSourceNumberPad;
		NumberPad *pLimitNumberPad;
		VoltageRangePad *pVoltageRangePad;


		void OnSetSourceClick(void);
		void OnSetLimitClick(void);
		void OnRangeClick(void); 

};





class SecondaryPanel: public ContainerClass 
{
	public:
		SecondaryPanel( int x, int y,  int w, int h);


		GraphClass *Graph;
		StatisticsPanel *Statistics; 

		CurrentMeasurmentPanel *CurrentMeasure;
		VoltageMeasurmentPanel *VoltageMeasure;

		const char* Classname() override {return "SecondaryPanel";}
		void Render()  override;

		void TouchStart(int x, int y ) override {SwipeController->TouchStart(x,y);};
		void TouchSwipe(int x, int y, int dx, int dy) override;
		void TouchEnd(int x, int y ) override {SwipeController->TouchEnd(x,y); };

		void SetHeaderTitle();

	protected: 
		LabelClass *TitleLabel; 

		SwipeControllerClass* SwipeController;

		void DrawBall(int x, int y, bool set);


};


class DisplayClass: public ContainerClass 
{
	public:
		DisplayClass();

		void ReadInput(); // read touch pnputs 
		void Render() override;
   	    const char* Classname() override {return "Display";}
		PrimaryPanel *pPrimary; 
		SecondaryPanel *pSecondary; 

		void OnTouchStart(int x, int y);
		void OnTouchEnd(int x, int y); 
		void OnTouchSwipe(int x, int y, int dx, int dy); 

		void KeyPress(); 

		void ShowWindow(WindowClass *p );
		void HideWindow() { HideFocus(); pModalWindow = nullptr; }

		void SetFocus(ControlClass *pControl) { pFocus = pControl; }
		void HideFocus() { pFocus = nullptr;}

		void SetLightOn100() {BackLight = 100; SetBackLight( BackLight); }; 
		void SetLightOn75() {BackLight = 75; SetBackLight( BackLight); }; 
		void SetLightOn50() {BackLight = 50; SetBackLight( BackLight); }; 
		void SetLightOn25() {BackLight = 25; SetBackLight( BackLight); }; 
		void SetLightOff() {BackLight = 0; SetBackLight( BackLight); }; 

	protected:
		ControlClass *pFocus = nullptr; 
		WindowClass *pModalWindow = nullptr;

		void RenderFocus();
		
		InputClass Input;
        
		void SetBackLight(uint v) { GD.wr(REG_PWM_DUTY, v);}
		uint BackLight = 50;
};


extern DisplayClass Display;

#endif

