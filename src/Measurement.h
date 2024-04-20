#ifndef MEASUREMENT_CLASS_h
#define MEASUREMENT_CLASS_h

#include "Arduino.h"

#define MAX_CALIBATION_SIZE 10
#define FILTER_SIZE 5


#define MAX_RANGES 10




enum RangeType
{
    //-------------------
    VoltageAuto,    // 0
    Voltage200mV,   
    Voltage4V,      
    Voltage30V,     
    
    //------------------
    CurrentAuto,    // 4
    Current1mA,     
    Current100mA,   
    Current1A,       

    //------------------

    ResistorAuto,    // 8
    Resistor1R,    
    Resistor100R,    
    Resistor1k,    
    Resistor10k,    
    Resistor100k,    
    Resistor1M,    
    Resistor10M,    
    
};


enum MeasurementType 
{
    VoltageType,
    CurrentType,
    PowerType,
    ResistorType
};


struct CalibrationConfig
{
	uint32_t p_code;     // DAC/ADC-Code	 
	float  p_value;		 // y-value
};



class AdjustmentClass
{
 protected:
	 CalibrationConfig Calibration[MAX_CALIBATION_SIZE];
	 int GetIdxDigital(float value);
	 int GetIdxFloat(uint32_t value);
	 int _Counter = 0;	
	 float FloatOffset;


	 // WINDOW AVERAGE Filter
	 //--------------------
	 float FilterSum;
	 float FilterBuffer[FILTER_SIZE];
	 float *FilterPosition; 		// This is the actual position of the ring buffer.

	 void FilterInit();
	 float Filter(float new_value);


 public:
	
	AdjustmentClass();

	uint32_t ToDigital(float value);
	float ToFloat(uint32_t value);

	int GetCount();

	void Add(CalibrationConfig config);

	void Setup();

	void Trace();
};


class RangeClass
{
    public:

    RangeClass( MeasurementType type, RangeType rangeType,  float min, float max, const char* text, uint8_t pin=0);


    void AddInAdjustment( uint32_t codeValue, float floatValue, bool isNegativ=false);
    void AddOutAdjustment( uint32_t codeValue, float floatValue, bool isNegativ=false);



    void SetMin( float value);
    float GetMin() ;

    void SetMax( float value); 
    float GetMax() ;

    void Unselect();
    void Select();

    const char* GetText() { return Text;}

    RangeType GetRangeType(void ) { return Range;}

    float ConvertToFloat( int32_t code);
    int32_t ConvertToDigital( float v);

    protected:
        const char* Text;   // Wir für den Range-Button benötigt.
        float Max;          // max Value in diesem Range
        float Min;          // min Value in diesem Range
        RangeType Range;

        int32_t MinCode; // adc code min in diesem Range
        int32_t MaxCode; // adc code max in diesem Range

        MeasurementType Type = VoltageType;
        uint8_t Pin =0;

        AdjustmentClass *ptrInAdjustment;
        AdjustmentClass *ptrOutAdjustment;      
        


};


class MeasurementClass
{
    public:

        MeasurementClass( MeasurementType type, int32_t maxADC, int32_t minADC);
        RangeClass* AddRange( RangeClass *pRange);

     

     
        MeasurementType Type; // Aufgabe des Channels ( Voltage, Current )

        float ConvertToFloat(int32_t code);    // 2 Punktabgleich für den digitalen Code
        int32_t ConvertToDigital( float v);    // 2 Punktabgleich für den digitalen Code
       
        uint32_t MaxADC;  // maximaler ADC Wert, danach Overflow
        uint32_t MinADC;  // minimaler ADC Wert, danach Overflow  



        // --- Rangeverwaltung -----  

        boolean virtual SetRange( RangeType range);         
        RangeClass* GetSelectedRange();  
        int GetRangeCount() { return CountRange;}
        RangeClass *GetRange(int idx) 
        {
            return ( idx < 0 && idx >= CountRange) ? nullptr : Ranges[idx];
        }


    protected:

  

       
        void UnselectRanges( void); 
          
        bool IsOutOfRange( float value);
        float GetMax();
        float GetMin();

        int CurrentRange =0;
        RangeClass *Ranges[MAX_RANGES];   
        int CountRange = 0;



};

class CurrentMeasurementClass: public MeasurementClass 
{
    public:
    //CurrentMeasurementClass( MeasurementType type, int32_t maxADC, int32_t minADC);

};

class VoltageMeasurementClass: public MeasurementClass 
{
    public:
    //VoltageMeasurementClass( MeasurementType type, int32_t maxADC, int32_t minADC);

};


class ResistorMeasurementClass: public MeasurementClass 
{
    public:
    ResistorMeasurementClass( MeasurementType type, int32_t maxADC, int32_t minADC);


};

class PowerMeasurementClass: public MeasurementClass 
{
    public:
    // PowerMeasurementClass( MeasurementType type, int32_t maxADC, int32_t minADC);


};


#endif