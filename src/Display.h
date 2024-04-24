

#ifndef _Display_h
#define _Display_h

#include "arduino.h"
#include <GD2.h>
#include "Controls.h"
#include "System.h"
#include "Display/MeasurmentPanel.h"
#include "Display/SourcePanel.h"



class InputClass 
{
	public:
		int StartX;   // Press und Release Position
		int StartY;   // Press und Release Position 

		int TrackX;
		int TrackY;

		ControlClass *pTouchObj = nullptr;

		uint8_t TouchState = 0; // 0=Kein Input // 1=Pressed // 2=Released

};




//********************************************************************
//	GUI 
//********************************************************************


#define MAX_INPUT_LENGTH 6






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

		CurrentMeasurmentPanel *CurrentMeasure;
		VoltageMeasurmentPanel *VoltageMeasure;
		ResistorMeasurmentPanel *ResistorMeasure;
		PowerMeasurmentPanel *PowerMeasure;	

		ButtonClass *FunctionButton;

	
	protected: 
	
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






class SecondaryPanel: public ContainerClass 
{
	public:
		SecondaryPanel( int x, int y,  int w, int h);


		GraphClass *Graph;
		StatisticsPanel *Statistics; 

		VoltageSourcePanel *VoltageSource;

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

		// Ein Simple Subscriber zur Event-Weiterleitung
		void RegisterSubscriber( ContainerClass *p) { pSubscriber = p; }

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

		ContainerClass *pSubscriber = nullptr; 

		void RenderFocus();
		
		InputClass Input;
        
		void SetBackLight(uint v) { GD.wr(REG_PWM_DUTY, v);}
		uint BackLight = 50;
};


extern DisplayClass Display;

#endif

