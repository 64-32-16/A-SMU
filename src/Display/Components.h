
#ifndef _Display_Components_h
#define _Display_Components_h

#include "arduino.h"
#include <GD2.h>
#include "Controls.h"
//#include "System.h"
//#include "Display.h"
//#include "MeasurmentPanel.h"



/**
 * @brief 
 * Über dieses PopUp Window kann der Bereich für die
 * Strommessung ausgewählt werden.
 * 
 */
class CurrentRangePad: public WindowClass 
{
	public:
		
		LabelClass *LabelTitle;		
		ButtonClass *BtnClose;

		ButtonClass *BtnAuto;
		ButtonClass *BtnRange1mA;
		ButtonClass *BtnRange100mA;
		ButtonClass *BtnRange1A;

		const char* Classname() override {return "CurrentRangePad";}

		CurrentRangePad(  int x, int y, int w, int h);

		void Render()  override;
		void Show() override;

	protected:
		void AddListner(void);
		void AddControls();
		void OnKeyClose(void);
		void OnKey1mA(void);
		void OnKey100mA(void);
		void OnKey1A(void);
};


class VoltageRangePad: public WindowClass 
{
	public:
		
		LabelClass *LabelTitle;		
		ButtonClass *BtnClose;

		ButtonClass *BtnAuto;
		ButtonClass *BtnRange200mV;
		ButtonClass *BtnRange4V;
		ButtonClass *BtnRange30V;

		const char* Classname() override {return "VoltageRangePad";}

		VoltageRangePad(  int x, int y, int w, int h);

		void Render()  override;

		void Show() override;

	protected:
		void AddListner(void);
		void AddControls();
		void OnKeyClose(void);
		void OnKey4V(void);
		void OnKey30V(void);
		
};



#endif