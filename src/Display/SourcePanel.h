#ifndef _Source_Panel_h
#define _Source_Panel_h


#include "arduino.h"
#include <GD2.h>
#include "Controls.h"
#include "Components.h"

class SourcePanel: public ContainerClass 
{

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
		RangePadClass *pRangePad;


		void OnSetSourceClick(void);
		void OnSetLimitClick(void);
		void OnRangeClick(void); 

};

#endif