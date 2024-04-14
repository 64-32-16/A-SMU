
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



		LabelClass *ValueLabel; 
		LabelClass *TitleLabel; 



		LabelClass *RangeLabel;
		ButtonClass *RangeButton;

	
	protected: 
	
};


/**
 * @brief 
 * 
 */
class CurrentMeasurmentPanel: public MeasurmentPanel 
{
	public:
		CurrentMeasurmentPanel( int x, int y,  int w, int h);

		CurrentRangePad *pCurrentRangePad;
		void Render()  override;
		const char* Classname() override {return "CurrentMeasurmentPanel";}

		

	
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

		VoltageRangePad *pVoltageRangePad;
		void Render()  override;
		const char* Classname() override {return "VoltageMeasurmentPanel";}

		

	
	protected: 
		void OnRangeClick();
};


#endif
