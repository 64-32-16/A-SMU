

#ifndef _SystemClass_h
#define _SystemClass_h

#include "Arduino.h"
#include "SPI.h"
#include "Measurement.h"
#include "ADS1220.h"

#include "DAC856X.h"



enum SourceLineType 
{
    VoltageSourceType,
    CurrentSourceType
};


struct ADC_Data 
{
   float Value;
   bool IsOverflow;
   bool IsReady; 
};


  struct BufferDataClass {
    float Voltage;
    float Current;
    float Power;
    float Resistor;
    float Time; // in ms
  };


class INumberPad 
{
    public:
        virtual float GetValue(void) = 0;
        virtual bool SetValue( float v) = 0;
        virtual bool ValidateValue(float v) = 0;
        virtual SourceLineType GetSourceType(void) = 0;
};


#define BUFFER_SIZE 1024


class BufferClass 
{
  
	public:
        BufferClass() {};

	
		void Add(long time, float voltage, float current);
		int GetData(BufferDataClass buffer[], uint16_t bufferSize );	
		uint16_t GetSize() { return BUFFER_SIZE;}

        int16_t GetCount() { return (Head+1);}

		bool IsEmpty() {return (Head == -1);}
		
        void Reset() { Head = -1;}

        // Current
        float GetCurrentMin() {return CurrentMin;}
		float GetCurrentMax() {return CurrentMax;}
        float GetCurrent() {return Current;}
        float GetCurrentPeakToPeak() {return (CurrentMax - CurrentMin); }
        float GetCurrentAverage(); 

        // Voltage
        float GetVoltageMin() {return VoltageMin;}
		float GetVoltageMax() {return VoltageMax;}
        float GetVoltage() { return Voltage;}
        float GetVoltagePeakToPeak() {return (VoltageMax - VoltageMin); }
        float GetVoltageAverage(); 

        // Power 
        float GetPowerMin() { return (CurrentMin*VoltageMin); }
        float GetPowerMax() { return (CurrentMax*VoltageMax); }
        float GetPower() { return (Current*Voltage); }
        float GetPowerPeakToPeak() {return (GetPowerMax() - GetPowerMin()); }
        

        // Resistor 
        float GetResistorMin() { return ( 0.0); }
        float GetResistorMax() { return (0.0); }
        float GetResistor() { 
            return (Voltage == 0.0) ? __FLT_MAX__ : (Voltage/Current); 
        }
        float GetResistorPeakToPeak() {return (0.0); }

		int SampleRate() { return 10;} // in ms


	protected:
		BufferDataClass Data[BUFFER_SIZE];

        
		int Head = -1;	
		float CurrentMin = 0.0;
		float CurrentMax = 0.0;

		float VoltageMin = 0.0;
		float VoltageMax = 0.0;

        float Voltage = 0.0;
        float Current = 0.0;


		long Time = 0;

};

//----------------------------------------------------------------
// Limit-Class
//----------------------------------------------------------------


class LimitClass: public INumberPad 
{
    public:
        LimitClass(SourceLineType sourceType, uint dacCannel);

        // Interface Member
        bool SetValue(float value);
        float GetValue() {return Limit; }        
        bool ValidateValue(float value );
        SourceLineType GetSourceType(void) {return SourceType; };

     

    protected:
        SourceLineType SourceType = VoltageSourceType;
        float Limit = 0.0;
 
        uint DacChannel;
        MeasurementClass *GetMeasurement();


};


//----------------------------------------------------------------
// Sourcce-Class
//----------------------------------------------------------------


class SourceClass: public INumberPad 
{
    public:
        SourceClass(SourceLineType sourceType, uint pin, bool pinState, uint dacCannel);

        bool SetValue(float value);
        float GetValue() {return Source; }        
        bool ValidateValue(float value );
        SourceLineType GetSourceType(void) {return SourceType; };

        void SetActive( void );

        //void SetLimit( LimitClass *limit) {pLimit = limit;}
        LimitClass *GetLimit( void) { return pLimit;}
        

    protected:
        LimitClass *pLimit;
        SourceLineType SourceType = VoltageSourceType;
        float Source = 0.0;
        uint Pin;
        bool PinState;
        uint DacChannel;
        MeasurementClass *GetMeasurement();


};




//----------------------------------------------------------------
// Sourcce-Class
//----------------------------------------------------------------

class SystemClass
{
    public:
        SystemClass();    


        const char* GetInfo() {return Info;}
        bool IsSystemOn() { return SystemOn; }
        bool SetSystemOn();
        bool SetSystemOff();

        void SetCurrentSET( float v);
        float GetCurrentSET();

        BufferClass Buffer;

        void Random();

        void SetDAC( uint16_t code, uint channel );

        String FormatCurrent( float value);
        String FormatVoltage( float value);
        String FormatPower( float value);
         String FormatResistor( float value);
        
        String FormatBufferCount();

        void Begin();
        void Execute(); 

        boolean IsSimulation = false;


        // Measuring
         void SetSelectedMeasuring( MeasurementType value );
        MeasurementClass *GetCurrentMeasurement() {return CurrentMeasurement; };
        MeasurementClass *GetVoltageMeasurement() {return VoltageMeasurement; };
        MeasurementClass *GetResistorMeasurement() {return ResistorMeasurement; };
        MeasurementClass *GetPowerMeasurement() {return PowerMeasurement; };
        MeasurementClass *GetSelectedMeasurement() {return SelectedMeasurement; };



        // Source 
        void SetSelectedSource( SourceLineType value );
        SourceClass *GetSelectedSource(void) { return SelectedSource; };
        SourceClass *GetVoltageSource(void) {return VoltageSource;};
        SourceClass *GetCurrentSource(void) {return CurrentSource;};
        
        

    protected:

        Protocentral_ADS1220 *pADS1220;
        DAC856XClass *pDAC;

        const char* Info = "AW-INSTRUMENTS,MODEL ASMU-100,000,1.0.0\n";
        
        bool SystemOn = false;
        SPIClass *SPI1;

        // ADC Functions

        ADC_Data ADCChannel01;
        ADC_Data ADCChannel02;
        bool IsDataReadyADC();
        void ResumeADC();
        void ReadDataADC();

         
        MeasurementClass *CurrentMeasurement;
        MeasurementClass *VoltageMeasurement;
        MeasurementClass *ResistorMeasurement;
        MeasurementClass *PowerMeasurement;
        MeasurementClass *SelectedMeasurement;
        

        // Source
        SourceClass *VoltageSource;
        SourceClass *CurrentSource;
        SourceClass *SelectedSource;


        void FormatValue(float value, char const* unit, bool scale = false);

        String FormatStr;

        uint32_t micros_end = 0;
uint32_t current_millis = 0;





};













extern SystemClass System;

#endif
