

#include "System.h"

//------------------------------------------------------
// SourceClass
//------------------------------------------------------

SourceClass::SourceClass(SourceLineType sourceType, uint pin, bool pinState, uint dacChannel)
{
	SourceType = sourceType;
	Pin = pin;
	PinState = pinState;
	DacChannel = dacChannel;

	if (sourceType == VoltageSourceType)
	{
		pLimit = new LimitClass(CurrentSourceType, 1);
	}
	else
	{
		pLimit = new LimitClass(VoltageSourceType, 1);
	}
}

// Mit dieser Funktion wird zwischen Spannungs- und Strom-Regelung umgeschaltet.
void SourceClass::SetActive(void)
{
	if (Pin == 0)
		return;
	digitalWrite(Pin, PinState);
}

bool SourceClass::SetValue(float v)
{
	bool result = false;

	if (ValidateValue(v) == true)
	{
		MeasurementClass *line = GetMeasurement();

		if (line != nullptr)
		{
			uint32_t code = line->ConvertToDigital(v); // Über den aktuelle Range float to Dac-Code
			Serial.print("Source DAC ");
			Serial.print(v);
			Serial.print(" code ");
			Serial.print(code, HEX);
			Serial.print(" Channel ");
			Serial.println(DacChannel);
			System.SetDAC(code, DacChannel);
			Source = v;
			result = true;
		}
		else
		{
			// TODO: Protokolleintrag
			Serial.println("[Error] no Line found ");
		}
	}

	return result;
}

bool SourceClass::ValidateValue(float v)
{

	MeasurementClass *line = GetMeasurement();
	if (line == nullptr)
		return false;

	RangeClass *range = line->GetSelectedRange();
	if (range == nullptr)
		return false;

	bool result = false;
	if (v < range->GetMin() || v > range->GetMax())
	{
		result = false;
	}
	else
	{
		result = true;
	}

	return result;
}

MeasurementClass *SourceClass::GetMeasurement()
{
	MeasurementClass *line = nullptr;
	if (SourceType == VoltageSourceType)
		line = System.GetVoltageMeasurement();
	if (SourceType == CurrentSourceType)
		line = System.GetCurrentMeasurement();

	return line;
}

//------------------------------------------------------
// Limit-Class
//------------------------------------------------------

LimitClass::LimitClass(SourceLineType sourceType, uint dacChannel)
{
	SourceType = sourceType;
	DacChannel = dacChannel;
}

bool LimitClass::SetValue(float v)
{
	bool result = false;

	if (ValidateValue(v) == true)
	{
		MeasurementClass *line = GetMeasurement();

		if (line != nullptr)
		{
			uint32_t code = line->ConvertToDigital(v); // Über den aktuelle Range float to Dac-Code
			Serial.print("Limit DAC ");
			Serial.print(v);
			Serial.print(" CODE ");
			Serial.print(code, HEX);
			Serial.print(" CHANNEL ");
			Serial.println(DacChannel);
			System.SetDAC(code, DacChannel);
			Limit = v;
			result = true;
		}
		else
		{
			// TODO: Protokolleintrag
			Serial.println("[Error] no Line found ");
		}
	}

	return result;
}

// Über die Messung den aktuellen Bereich abfragen
// und Validierung der Eingabe setzten.
bool LimitClass::ValidateValue(float v)
{

	MeasurementClass *line = GetMeasurement();
	if (line == nullptr)
		return false;

	RangeClass *range = line->GetSelectedRange();
	if (range == nullptr)
		return false;

	bool result = false;
	if (v < range->GetMin() || v > range->GetMax())
	{
		result = false;
	}
	else
	{
		result = true;
	}

	return result;
}

MeasurementClass *LimitClass::GetMeasurement()
{
	MeasurementClass *line = nullptr;
	if (SourceType == VoltageSourceType)
		line = System.GetVoltageMeasurement();
	if (SourceType == CurrentSourceType)
		line = System.GetCurrentMeasurement();

	return line;
}

//------------------------------------------------------
// SystemClass
//------------------------------------------------------

SystemClass::SystemClass()
{

	// TODO Pin setzten
	VoltageSource = new SourceClass(VoltageSourceType, 0, HIGH, 0);
	CurrentSource = new SourceClass(CurrentSourceType, 0, LOW, 0);
	SetSelectedSource(VoltageType);

	// Current Measurement
	// Max- und Min-Bereich vom ADC werden übergeben.
	// Jede Messung hat mindestens einen Range.
	CurrentMeasurement = new MeasurementClass(CurrentType, 0x7FFFFF - 1, 0x00);
	{
		RangeClass *ir1 = new RangeClass(CurrentType, Current1A, -1.00, 1.00, "1A");
		{

			ir1->AddInAdjustment(0x270B60, +0.407); // NUR TEST
			ir1->AddInAdjustment(0x4C3CD3, 0.0);	// NUR TEST
			ir1->AddInAdjustment(0x716068, -0.411); // NUR TEST

			ir1->AddOutAdjustment(0x00, 0.0);		  // Adj for DAC (16 Bit)
			ir1->AddOutAdjustment(0x10000 - 1, 1.00); // Adj for DAC (16 Bit)
		}
		RangeClass *ir2 = new RangeClass(CurrentType, Current100mA, -0.10, 0.10, "100mA");
		{
			ir2->AddInAdjustment(0x00, 2.048);		// NUR TEST
			ir2->AddInAdjustment(0x2F9C3A, 0.0);	// NUR TEST
			ir2->AddInAdjustment(0x6CC07E, -2.048); // NUR TEST
		}
		RangeClass *ir3 = new RangeClass(CurrentType, Current1mA, -0.001, 0.001, "1mA");
		{

			ir3->AddInAdjustment(0x00, 2.048);		// NUR TEST
			ir3->AddInAdjustment(0x2F9C3A, 0.0);	// NUR TEST
			ir3->AddInAdjustment(0x6CC07E, -2.048); // NUR TEST
		}
		CurrentMeasurement->AddRange(ir1);
		CurrentMeasurement->AddRange(ir2);
		CurrentMeasurement->AddRange(ir3);
		CurrentMeasurement->SetRange(Current1A);
	}

	// Voltage Measurement
	// Max- und Min-Bereich vom ADC werden übergeben.
	// Jede Messung hat mindestens einen Range.
	VoltageMeasurement = new MeasurementClass(VoltageType, 0x7FFFFF - 1, 0x00);
	{
		RangeClass *vr1 = new RangeClass(VoltageType, Voltage4V, -4.0, 4.0, "4V");
		{
			vr1->AddInAdjustment(0x00, 2.048);		// NUR TEST
			vr1->AddInAdjustment(0x2F9C3A, 0.0);	// NUR TEST
			vr1->AddInAdjustment(0x6CC07E, -2.048); // NUR TEST
		}
		RangeClass *vr2 = new RangeClass(VoltageType, Voltage30V, -24.0, 24.0, "30V");
		{

			vr2->AddInAdjustment(0x19D39A, +20.0001); // NUR TEST
			vr2->AddInAdjustment(0x4C9C3F, 0.0000);	  // NUR TEST
			vr2->AddInAdjustment(0x7F668B, -20.0001); // NUR TEST

			// DAC
			vr2->AddOutAdjustment(0x561C, -16.5024); // Adj for DAC (16 Bit)

			vr2->AddOutAdjustment(0x807C, +0.046); // Adj for DAC (16 Bit)

			vr2->AddOutAdjustment(0xAABA, +16.5444); // Adj for DAC (16 Bit)
		}
		VoltageMeasurement->AddRange(vr1);
		VoltageMeasurement->AddRange(vr2);
		VoltageMeasurement->SetRange(Voltage30V);
	}

	ResistorMeasurement = new ResistorMeasurementClass(ResistorType, 0x7FFFFF - 1, 0x00);

}

// Mit dieser Funktion kann zwischen Voltage- und Current-Source umgeschaltet werden.
void SystemClass::SetSelectedSource(MeasurementType line)
{
	if (line == VoltageType)
		SelectedSource = VoltageSource;
	if (line == CurrentType)
		SelectedSource = CurrentSource;

	SelectedSource->SetActive();
}

void SystemClass::SetDAC(uint16_t code, uint channel)
{
	if (pDAC != nullptr)
	{

		if (channel == 0)
			pDAC->WriteValue(CMD_SETA_UPDATEA, code);
		if (channel == 1)
			pDAC->WriteValue(CMD_SETB_UPDATEB, code);
	}
}

bool SystemClass::SetSystemOn()
{
	SystemOn = true;

	return true;
}

bool SystemClass::SetSystemOff()
{
	SystemOn = false;

	return true;
}

// Simulation, falls keine ADC vorhanden ist.
void SystemClass::Random()
{
	float v = (random(-1000, 1001) / (1000.0 * 1000.0));
	float c = (random(-1000, 1001) / (1000.0 * 1000.0));

	if (random(0, 11) == 10)
		c = (random(-1000, 1001) / (10.0 * 1000.0));

	Buffer.Add(0, (2.0 + v), (1 + c));
}

void SystemClass::Begin()
{
	if (IsSimulation)
		return;

	SPI1 = new SPIClass(HSPI); // SPI Interface für DAC und ADC

	pADS1220 = new Protocentral_ADS1220(SPI1);
	pADS1220->begin(5, 27); // cs_pin = 5, dataReadyPin = 27

	pADS1220->set_data_rate(DR_45SPS);
	pADS1220->set_pga_gain(PGA_GAIN_1);
	pADS1220->set_FIR_Filter(FIR_50Hz);
	pADS1220->select_mux_channels(MUX_AIN0_AVSS); // Configure AIN0 and AVSS
	pADS1220->select_mux_channels(MUX_AIN1_AVSS); // Configure AIN0 and AVSS
	pADS1220->set_conv_mode_continuous();		  // Set continuous conversion mode
	pADS1220->Start_Conv();						  // Start continuous conversion mode

	pDAC = new DAC856XClass(SPI1);
	pDAC->Begin(22);

	ResumeADC();
}

void SystemClass::Execute()
{
	if (IsSimulation)
	{

		// Update display
		if ((millis() - micros_end) > 200)
		{
			micros_end = millis();
			Random();

			
		}
		return;
	}

	// ADC Funktionen
	ReadDataADC();
	if (IsDataReadyADC())
	{
		// Channel 01 = Voltage
		// Channel 02 = Current;

		Buffer.Add(0, ADCChannel01.Value, ADCChannel02.Value);
		ResumeADC();
	}
}

/*---------------------------------------------------------------------
	ADC
	Die Messung am ADC erfolgt als SINGLE END.
	Nach jeder Messung, wir der MUX auf den nächsten Channel umgeschaltet.
	Zur Zeit werden nur zwei Channel (Strom und Spannung) verwendet.
----------------------------------------------------------------------*/

void SystemClass::ReadDataADC()
{

	if (IsDataReadyADC() == true)
		return; // wir warten, bis beide Channel gelesen wurden.

	if (pADS1220->IsDataReady())
	{

		uint32_t n = pADS1220->Read_Data_Samples();

		uint8_t c = pADS1220->get_mux_channel();

		switch (c)
		{
		case MUX_AIN0_AVSS: // Voltage-Measuring
		{
			// Serial.print("CHANNEL 01 ADC-code :"); Serial.println(n, HEX);
			ADCChannel01.Value = VoltageMeasurement->ConvertToFloat(n);
			ADCChannel01.IsReady = true;
			pADS1220->select_mux_channels(MUX_AIN1_AVSS);
			break;
		}
		case MUX_AIN1_AVSS: // Current-Measuring
		{
			// Serial.print("CHANNEL 02 ADC-code :"); Serial.println(n, HEX);
			//  TODO Kontrollieren
			ADCChannel02.Value = CurrentMeasurement->ConvertToFloat(n);
			ADCChannel02.IsReady = true;
			pADS1220->select_mux_channels(MUX_AIN0_AVSS);
			break;
		}
		default:
			pADS1220->select_mux_channels(MUX_AIN0_AVSS);
		}
	}
}

bool SystemClass::IsDataReadyADC()
{
	return (ADCChannel01.IsReady && ADCChannel02.IsReady);
}

void SystemClass::ResumeADC()
{
	ADCChannel01.IsReady = false;
	ADCChannel02.IsReady = false;
}

//------------------------------------------------------
// Format Funktionen
//------------------------------------------------------

String SystemClass::FormatBufferCount()
{
	FormatStr = "";
	FormatStr = String(Buffer.GetCount());
	return FormatStr;
}

// TODO Verlagern in eine static Helper-Class
void SystemClass::FormatValue(float value, char const *unit, bool scale)
{

	if (value == __FLT_MAX__)
	{
		FormatStr.concat(String("Overflow"));
		FormatStr.concat(unit);
		return;
	}

	if (value >= 0.0)
	{
		FormatStr.concat("+");
	}
	else
	{
		FormatStr.concat("-");
	}

	value = abs(value);

	if (value < pow(10, -3)) // < 1 m
	{

		FormatStr.concat(String((value * pow(10, 6)), 3));
		FormatStr.concat("u");
		FormatStr.concat(unit);
	}
	else
	{
		if (value < pow(10, -1))
		{

			FormatStr.concat(String((value * pow(10, 3)), 3));
			FormatStr.concat("m");
			FormatStr.concat(unit);
		}
		else
		{
			FormatStr.concat(String((value), 5));
			FormatStr.concat(unit);
		}
	}
}

String SystemClass::FormatCurrent(float value)
{
	FormatStr = "";
	FormatValue(value, "A");
	return FormatStr;
}

String SystemClass::FormatVoltage(float value)
{
	FormatStr = "";
	FormatValue(value, "V");
	return FormatStr;
}

String SystemClass::FormatPower(float value)
{
	FormatStr = "";
	FormatValue(value, "W");
	return FormatStr;
}

//------------------------------------------------------
// BufferClass
//------------------------------------------------------

float BufferClass::GetCurrentAverage()
{
	float sum = 0.0;
	for (int i = 0; i < Head; i++)
	{
		sum = sum + Data[i].Current;
	}
	return (sum / (Head));
}

float BufferClass::GetVoltageAverage()
{
	float sum = 0.0;
	for (int i = 0; i < Head; i++)
	{
		sum = sum + Data[i].Voltage;
	}
	return (sum / (Head));
}

void BufferClass::Add(long time, float voltage, float current)
{
	if ((millis() - Time) > SampleRate())
	{

		Voltage = voltage;
		Current = current;

		// Overflow abgfangen
		if (voltage == __FLT_MAX__ || current == __FLT_MAX__)
			return;

		if ((Head + 1) < GetSize())
		{

			if (IsEmpty())
			{
				VoltageMin = voltage;
				VoltageMax = voltage;
				CurrentMin = current;
				CurrentMax = current;
			}
			else
			{

				if (voltage < VoltageMin)
					VoltageMin = voltage;
				if (voltage > VoltageMax)
					VoltageMax = voltage;
				if (current < CurrentMin)
					CurrentMin = current;
				if (current > CurrentMax)
					CurrentMax = current;
			}
			Head = Head + 1;

			Data[Head].Voltage = voltage;
			Data[Head].Current = current;
			Data[Head].Power = (voltage * current);
			Data[Head].Resistor = (current == 0.0) ? 0.0 : (voltage / current);

			Data[Head].Time = time;

			// Serial.print( "MIN C ");Serial.print(  CurrentMin, 6 );  Serial.print(  " MAX C ");Serial.print(  CurrentMax, 6 );  Serial.print(  " Current ");Serial.println(  current,6 );
		}
		Time = millis();
	}
}

/*
	Ab der aktuellen Head-Position werden die Daten rückwerts gelesen.
	Der Return-Wert zeigt, wieviele Daten gelesen werden konnten.
*/
int BufferClass::GetData(BufferDataClass buffer[], uint16_t bufferSize)
{
	int c = 0;

	if (IsEmpty())
		return c;

	int start = ((Head - bufferSize) <= 0) ? 0 : (Head - bufferSize);

	for (int idx = start; idx < Head; idx++)
	{
		buffer[c].Voltage = Data[idx].Voltage;
		buffer[c].Current = Data[idx].Current;
		buffer[c].Power = Data[idx].Power;
		buffer[c].Resistor = Data[idx].Resistor;

		buffer[c].Time = Data[idx].Time;
		c = c + 1;
	}
	return c;
}
