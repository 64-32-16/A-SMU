
#ifndef _Display_Components_h
#define _Display_Components_h

#include "arduino.h"
#include <GD2.h>
#include "Controls.h"



typedef  void (ControlClass::*SetRangeFn)( int idx); 

class RangeItemClass
{
public:
	RangeClass *Range;
	ButtonClass *Btn;
};


enum UnitBaseEnum 
{
	Base,
	Millis,
	Mikros

};



class WirePadClass: public WindowClass 
{
	public:
		WirePadClass(int x, int y, int w, int h);
		void OnKey2Wire( ButtonClass *btn); 
		void OnKey4Wire( ButtonClass *btn); 
		void OnKeyClose( ButtonClass *btn); 
		void Render() override;
		void Show() override;

	protected:
		ButtonClass *Btn2Wire;
		ButtonClass *Btn4Wire;
		ButtonClass *BtnClose;

		

		LabelClass *LabelTitle;
		
		
};


class MeasuringFunctionPadClass: public WindowClass 
{
	public:
		MeasuringFunctionPadClass(int x, int y, int w, int h);
		void OnKeyClose( ButtonClass *btn); 
		void OnKeyCurrent( ButtonClass *btn); 
		void OnKeyVoltage( ButtonClass *btn); 
		void OnKeyResistor( ButtonClass *btn); 
		void OnKeyPower( ButtonClass *btn); 
		void Render() override;
		void Show() override;

	protected:
		ButtonClass *BtnCurrent;
		ButtonClass *BtnVoltage;
		ButtonClass *BtnResistor;
		ButtonClass *BtnPower;
		ButtonClass *BtnClose;

		

		LabelClass *LabelTitle;
		
		
};


class RangePadClass: public WindowClass
{

public:
	RangePadClass(int x, int y, int w, int h, MeasurementClass *measuring);
	void Init(MeasurementClass *measuring);
	ButtonClass *BtnClose;
	LabelClass *LabelTitle;

	void OnRangeBtnClick( ButtonClass *r); 
	void OnKeyClose( void); // TODO Umstellen auf neuen Event
	 
	RangeClass * GetRangeByBtn( ButtonClass *btn );
	ButtonClass * GetBtnByRange( RangeClass *r );
	

	void Render() override;
	void Show() override;


protected: 
    RangeItemClass RangeItems[10];
	int RangeItemsCount = 0;

	MeasurementClass *pMeasuring = nullptr;
};

/**
 * @brief
 * Über dieses PopUp Window kann der Bereich für die
 * Strommessung ausgewählt werden.
 *
 */
class CurrentRangePad : public WindowClass
{
public:
	LabelClass *LabelTitle;
	ButtonClass *BtnClose;

	ButtonClass *BtnAuto;
	ButtonClass *BtnRange1mA;
	ButtonClass *BtnRange100mA;
	ButtonClass *BtnRange1A;

	const char *Classname() override { return "CurrentRangePad"; }

	CurrentRangePad(int x, int y, int w, int h);

	void Render() override;
	void Show() override;

protected:
	void AddListner(void);
	void AddControls();
	void OnKeyClose(void);
	void OnKey1mA(void);
	void OnKey100mA(void);
	void OnKey1A(void);
};

class VoltageRangePad : public WindowClass
{
public:
	LabelClass *LabelTitle;
	ButtonClass *BtnClose;

	ButtonClass *BtnAuto;
	ButtonClass *BtnRange200mV;
	ButtonClass *BtnRange4V;
	ButtonClass *BtnRange30V;

	const char *Classname() override { return "VoltageRangePad"; }

	VoltageRangePad(int x, int y, int w, int h);

	void Render() override;

	void Show() override;

protected:
	void AddListner(void);
	void AddControls();
	void OnKeyClose(void);
	void OnKey4V(void);
	void OnKey30V(void);
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


#endif