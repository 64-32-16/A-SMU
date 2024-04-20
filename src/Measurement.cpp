
#include "Measurement.h"



/*----------------------------------------------------------------------------------
    ResistorMeasurementClass
------------------------------------------------------------------------------------*/


ResistorMeasurementClass::ResistorMeasurementClass( MeasurementType type,  int32_t maxADC, int32_t minADC):
MeasurementClass( type,  maxADC, minADC) 
{
    // Add Range
     AddRange( new RangeClass(ResistorType, Resistor1R, 0.0, 1.0, "1R"));
     AddRange( new RangeClass(ResistorType, Resistor100R, 0.0, 100.0, "100R"));
     AddRange( new RangeClass(ResistorType, Resistor1k, 0.0, 1000.0, "1k"));
     AddRange( new RangeClass(ResistorType, Resistor10k, 0.0, (1000.0 * 10.0), "10k"));
     AddRange( new RangeClass(ResistorType, Resistor100k, 0.0, (1000.0 * 100.0), "100k"));
     AddRange( new RangeClass(ResistorType, Resistor1M, 0.0, (1000.0 * 1000.0), "1M"));
     AddRange( new RangeClass(ResistorType, Resistor10M, 0.0, (1000.0 * 10000.0), "10M"));

     SetRange( Resistor1k);

}







/*----------------------------------------------------------------------------------
    MeasurementClass
------------------------------------------------------------------------------------*/


MeasurementClass::MeasurementClass( MeasurementType type,  int32_t maxADC, int32_t minADC)
{
    Type = type;
    MaxADC = maxADC;
    MinADC = minADC;

}





RangeClass* MeasurementClass::AddRange( RangeClass *pRange)
 {  
        CountRange++;
        int idx = CountRange -1;
        Ranges[idx] = pRange;
        return pRange;
 }


 // aktuellen Range Bereich setzten
boolean MeasurementClass::SetRange( RangeType range)
{
    if( (Type == VoltageType && range >= 4 ) || (Type == CurrentType && range < 4 )  )   
    {
        Serial.println("[Error] RangeType not match with MeasurementType" );
        return false;
    }

    for( int idx=0; idx<= CountRange-1; idx++) 
    {
        if(  Ranges[idx]->GetRangeType() == range ) 
        {
            UnselectRanges();  
            CurrentRange = idx;
            Ranges[CurrentRange]->Select();
        }

    }
    return true;

}



void MeasurementClass::UnselectRanges( void) 
{

    for (int i=0; i<=CountRange-1; i++)
    {
         Ranges[i]->Unselect();
    }
}


float MeasurementClass::ConvertToFloat(int32_t code) 
{
    float result = __FLT_MAX__;

    //Serial.print("ADC-Code ");Serial.println(code, HEX);

    if( code > MaxADC || code > MaxADC  ) return result;

    RangeClass* range = GetSelectedRange();

    if( range != nullptr) 
    {
        result = range->ConvertToFloat( code);
    }
    return result;
}

int32_t MeasurementClass::ConvertToDigital( float v) 
{
	int32_t result = 0;
	 RangeClass* range = GetSelectedRange();

    if( range != nullptr) 
    {
        result = range->ConvertToDigital( v);
    }
	
	return result;
}




// Liefert den aktuellen Range
RangeClass* MeasurementClass::GetSelectedRange() 
{
    return Ranges[CurrentRange];
}







bool MeasurementClass::IsOutOfRange( float value) 
{
    
    return ( value > GetMax() || value < GetMin());
}




float MeasurementClass::GetMax() 
{

    if( GetSelectedRange() != NULL ) {
        return GetSelectedRange()->GetMax();
    } else 
    {
        // Vielleicht internen Fehler senden ??
        return 0.0;
    }
}

float MeasurementClass::GetMin() 
{
    if( GetSelectedRange() != NULL) 
    {
        return GetSelectedRange()->GetMin();
    } else 
    {
        return 0.0;
    }
}






/*----------------------------------------------------------------------------------
    RangeClass
------------------------------------------------------------------------------------*/


/* 
    Jede Messung hat mindestes einen Bereich (Range) 
*/
RangeClass::RangeClass( MeasurementType type, RangeType rangeType,  float min, float max, const char* text, uint8_t pin) 
{
    Text = text; 
    Type = type; 
    Min = min;
    Max = max;
    Pin = pin;
    Range = rangeType;

    pinMode( Pin, OUTPUT);
    Unselect();

    ptrInAdjustment = new AdjustmentClass();
    ptrOutAdjustment = new AdjustmentClass();

}




void RangeClass::AddInAdjustment( uint32_t codeValue, float floatValue, bool isNegativ)
{
    if( ptrInAdjustment != NULL) 
    {
        CalibrationConfig cfg = {codeValue, floatValue }; 
        ptrInAdjustment->Add( cfg );
    }

}

void RangeClass::AddOutAdjustment( uint32_t codeValue, float floatValue, bool isNegativ)
{
    if( ptrOutAdjustment != NULL) 
    {
        CalibrationConfig cfg = {codeValue, floatValue }; 
        ptrOutAdjustment->Add( cfg );
    }

}

int32_t RangeClass::ConvertToDigital(   float v) 
{
    int32_t value = ptrOutAdjustment->ToDigital(v);
	// TODO Validierung


    return value;    

}

float RangeClass::ConvertToFloat(  int32_t code) 
{
    float value = ptrInAdjustment->ToFloat(code);

    if( value > Max) value = __FLT_MAX__;

    return value;    

}

void RangeClass::Select()
{
    if( Pin > 0 ) 
    {
        digitalWrite(Pin, HIGH);
    }
}

void RangeClass::Unselect()
{
    if( Pin > 0 ) 
    {
        digitalWrite(Pin, LOW);
    }
}



void RangeClass::SetMin( float value) 
{
    Min = value;
}


float RangeClass::GetMin() 
{
    return Min;
}

void RangeClass::SetMax( float value) 
{
    Max = value;
}


float RangeClass::GetMax() 
{
    return Max;
}




/*----------------------------------------------------------------------------------
    AdjustmentClass
------------------------------------------------------------------------------------*/


// Berechnung der realen Werte inkl. Tiefpass Filter 
// 1er Ordung

AdjustmentClass::AdjustmentClass() 
{
	int i=0;
	// Ende Sequenz im Array setzten
	for(i=0; i< MAX_CALIBATION_SIZE; i++) 
	{
		Calibration[i].p_value = -1.0;
		Calibration[i].p_code = -1;
	}


	FilterInit();

	FloatOffset = 0.25;

}

void AdjustmentClass::FilterInit() 
{
  int i=0;
  FilterSum = 0.0;
  FilterPosition = FilterBuffer;
  for(i=0; i<FILTER_SIZE; i++) FilterBuffer[i] = 0.0;

}


void AdjustmentClass::Setup()
{


}


void AdjustmentClass::Add(CalibrationConfig config)
{
       _Counter++;
        int idx = _Counter -1;
        Calibration[idx] = config;
}




/*


*/
float AdjustmentClass::ToFloat(uint32_t code)
{

	int idx = GetIdxFloat(code);
    float result = 0.0;

   

	if (idx > 0)
	{
		CalibrationConfig p1 = Calibration[idx - 1];
		CalibrationConfig p2 = Calibration[idx];


        // P1:  y1 = m * x1 + b
        // P2:  y2 = m * x2 + b        
        // Formel        
        // b = y2 - (m * x2)
        // m = (y2-y1) / (x2-x1)
        // P3: y3 = m * x3 + b

		
		if( code < p1.p_code) code = p1.p_code;
		
        // Streigung berechnen. // m = (y2-y1) / (x2-x1)
		double gain = ( (p2.p_value - p1.p_value)/ (p2.p_code - p1.p_code)  );
		
        float offset = p2.p_value - ( gain * p2.p_code) ;
		
        // P3: y3 = m * x3 + b
        //-----------------------------

		result =  (gain * code ) + offset;
	}
	result = Filter(result);

	return result;
}





/* Digitalen Wert für den Float Wert bestimmen. 
*/
uint32_t AdjustmentClass::ToDigital(float value)
{

	int idx = GetIdxDigital(value);
	uint32_t dac_value = 0;

	//Serial.print("Index: "); Serial.println(idx);

	if (idx > 0)
	{
		CalibrationConfig p1 = Calibration[idx - 1];
		CalibrationConfig p2 = Calibration[idx];

		float gain = ((p2.p_value - p1.p_value) / (p2.p_code - p1.p_code));
		float offset = p2.p_value - ( gain * p2.p_code) ;

        dac_value =  (value-offset) / gain;
	}

	return dac_value;

}





// Liefert den obersten Stützpunkt im Array
// oder -1, wenn keine Datenvorhanden sind
int AdjustmentClass::GetIdxFloat(uint32_t value)
{
	
	int i = 0;
	int length = GetCount();
	
	// Wenn der erste Wert im Array schon kleiner ist
	// als der geforderte Wert, liefern wir die ersten beiden Werte
	// Ein Between ist nicht möglich
	if( i+1 < length) 
	{
		//-> Serial.println(value);
		//-> Serial.println(Calibration[0].p_value);

		if( value < Calibration[0].p_code )
		{
			return 1;
		}
	}
		
	for (i =0; i < length; i++)
	{
		if( i+1 < length) 
		{			
			// Die Stützpunkte zwischem diesem Wert bestimmen (between)
			if ((value >= Calibration[i].p_code) &&  (value <= Calibration[i + 1].p_code))
			{
				return 	i+1;
			}
		}

	}
	return length-1;
}



// Liefert den obersten Stützpunkt im Array
// oder -1, wenn keine Datenvorhanden sind
int AdjustmentClass::GetIdxDigital(float value)
{
	
	int i = 0;
	int length = GetCount();
	
	// Wenn der erste Wert im Array schon kleiner ist
	// als der geforderte Wert, liefern wir die ersten beiden Werte
	// Ein Between ist nicht möglich
	if( i+1 < length) 
	{
		//-> Serial.println(value);
		//-> Serial.println(Calibration[0].p_value);

		if( value < Calibration[0].p_value )
		{
			return 1;
		}
	}
		
	for (i =0; i < length; i++)
	{
		if( i+1 < length) 
		{			
			// Die Stützpunkte zwischem diesem Wert bestimmen (between)
			if ((value >= Calibration[i].p_value) &&  (value <= Calibration[i + 1].p_value))
			{
				return 	i+1;
			}
		}

	}
	return length-1;
}


int AdjustmentClass::GetCount()
{
	return _Counter;
}

void AdjustmentClass::Trace()
{
	Serial.println("TRACE");
	Serial.println(GetCount());
	float value = 0.0 * 1000.0;

	//for (int i; i < GetLength(); i++)
	//{
	//	Serial.println(Calibration[i].p_dac);
	//	Serial.println(Calibration[i].p_value);
	//}
	Serial.println("Find IDX");
	Serial.println(value);
	Serial.println( GetIdxDigital(value) );



	Serial.println("TRACE END");

}


// TODO: Filter sollte im Buffer sein ?!
//-----------------------------------

// https://atwillys.de/content/cc/digital-filters-in-c/

float AdjustmentClass::Filter(float new_value) {
  // Substract oldest value from the sum
  FilterSum -= *FilterPosition;
 
  // Update ring buffer (overwrite oldest value with new one)
  *FilterPosition = new_value;
 
  // Add new value to the sum
  FilterSum += new_value;
 
  // Advance the buffer write position, rewind to the beginning if needed.
  if(++FilterPosition >= FilterBuffer + FILTER_SIZE) {
    FilterPosition = FilterBuffer;
  }
 
  // Return sum divided by FILTER_SIZE, which is faster done by right shifting
  // The size of the ring buffer in bits. ( filter_sum / 2^bits ).
  return FilterSum / FILTER_SIZE;
}