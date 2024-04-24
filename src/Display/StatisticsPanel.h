#ifndef _Statistics_Panel_h
#define _Statistics_Panel_h


#include "arduino.h"
#include <GD2.h>
#include "Controls.h"
#include "Components.h"



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


#endif