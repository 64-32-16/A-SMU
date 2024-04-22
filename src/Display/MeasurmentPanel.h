
#ifndef _Measurment_Panel_h
#define _Measurment_Panel_h

#include "arduino.h"

#include "Controls.h"
#include "Components.h"




/**
 * @brief abstract base class for measurment 
 * 
 */
class MeasurmentPanel: public ContainerClass 
{
	public:
		MeasurmentPanel( int x, int y,  int w, int h);

		void Render()  override;
		const char* Classname() override {return "MeasurmentPanel";}

		RangePadClass *pRangePad;
		MeasuringFunctionPadClass *pFunctionPad;
		WirePadClass *pWirePad;

		LabelClass *ValueLabel; 
		LabelClass *TitleLabel; 
		LabelClass *RangeLabel;
		ButtonClass *RangeButton;
		ButtonClass *FunctionButton;
		ButtonClass *WireButton;

	
	protected: 


	void SetRage(RangeClass *range );	
	void OnFunctionClick(void);
	void OnWireClick(void);
};


/**
 * @brief 
 * 
 */
class CurrentMeasurmentPanel: public MeasurmentPanel 
{
	public:
		CurrentMeasurmentPanel( int x, int y,  int w, int h);

		void Render()  override;
		const char* Classname() override {return "CurrentMeasurmentPanel";}

	
	protected: 
		void OnRangeClick();
};



/**
 * @brief 
 * 
 */
class ResistorMeasurmentPanel: public MeasurmentPanel 
{
	public:
		ResistorMeasurmentPanel( int x, int y,  int w, int h);

		void Render()  override;
		const char* Classname() override {return "ResistorMeasurmentPanel";}

	
	protected: 
		void OnRangeClick();
};


/**
 * @brief 
 * 
 */
class VoltageMeasurmentPanel: public MeasurmentPanel 
{
	public:
		VoltageMeasurmentPanel( int x, int y,  int w, int h);
		
		void Render()  override;
		const char* Classname() override {return "VoltageMeasurmentPanel";}

		

	
	protected: 
		void OnRangeClick();
};


/**
 * @brief 
 * 
 */
class PowerMeasurmentPanel: public MeasurmentPanel 
{
	public:
		PowerMeasurmentPanel( int x, int y,  int w, int h);
		
		void Render()  override;
		const char* Classname() override {return "VoltageMeasurmentPanel";}

		

	
	protected: 
		void OnRangeClick();
};


#endif
