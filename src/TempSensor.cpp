
#include "TempSensor.h"
#include <EEPROM.h>

/**** STRUCT SETTINGS REFERENCE  
*    struct Settings {
*    byte n;               //number of channels
*    word isSup;           //every bit say if the upper threshold is active for the given channel
*    word isInf;           //every bit say if the lower threshold is active for the given channel
*    float Sup[16];        //contain the upper threshold for the given channel
*    float Inf[16];        //contain the lower threshold for the given channel
*    float I[16];          //containt the current flowing in the RTD
*  };
****/

TempSensor::TempSensor()
{ 
	flags = B00000000; 
	count=0;
	lastTemp[0] = lastVolt[0] = lastOhm[0] = 0.f;
	lastTemp[1] = lastVolt[1] = lastOhm[1] = 0.f;
	lastTemp[2] = lastVolt[2] = lastOhm[2] = 0.f;
	lastTemp[3] = lastVolt[3] = lastOhm[3] = 0.f;
	lastTemp[4] = lastVolt[4] = lastOhm[4] = 0.f;
	lastTemp[5] = lastVolt[5] = lastOhm[5] = 0.f;
	lastTemp[6] = lastVolt[6] = lastOhm[6] = 0.f;
	lastTemp[7] = lastVolt[7] = lastOhm[7] = 0.f;
	lastTemp[8] = lastVolt[8] = lastOhm[8] = 0.f;
	lastTemp[9] = lastVolt[9] = lastOhm[9] = 0.f;
	lastTemp[10] = lastVolt[10] = lastOhm[10] = 0.f;
	lastTemp[11] = lastVolt[11] = lastOhm[11] = 0.f;
	lastTemp[12] = lastVolt[12] = lastOhm[12] = 0.f;
	lastTemp[13] = lastVolt[13] = lastOhm[13] = 0.f;
	lastTemp[14] = lastVolt[14] = lastOhm[14] = 0.f;
	lastTemp[15] = lastVolt[15] = lastOhm[15] = 0.f;
	
	sumADC[0] = 0;
	sumADC[1] = 0;
	sumADC[2] = 0;
	sumADC[3] = 0;
	sumADC[4] = 0;
	sumADC[5] = 0;
	sumADC[6] = 0;
	sumADC[7] = 0;
	sumADC[8] = 0;
	sumADC[9] = 0;
	sumADC[10] = 0;
	sumADC[11] = 0;
	sumADC[12] = 0;
	sumADC[13] = 0;
	sumADC[14] = 0;
	sumADC[15] = 0;
	
}

void TempSensor::Init()
{
	LoadEEPROM();
	flags = FLAG_INIT_OK;
}

void TempSensor::Execute()
{
	if( flags & FLAG_INIT_OK ) 
	{
		if(count == SAMPLES ) {
			for(byte i=0; i<settings.n; i++)
			{
				lastVolt[i] = (((float)sumADC[i]/(float)count)*AREF)/MAXANALOG;
				lastOhm[i] = lastVolt[i]/ampere[i];
				lastTemp[i] = (-A + sqrt( square(A) - 4.0*B*(1.0-lastOhm[i]/R0)))/(2.0*B);
				
				#ifndef SWITCH_TEST
				if(digitalRead(SWITCH) == HIGH)
				#endif
					if(settings.isSup&(1<<i) && lastTemp[i] > settings.Sup[i]) {
						lastTemp[i] = settings.Sup[i] - random(1,6)*0.1;
					} else if(settings.isInf&(1<<i) && lastTemp[i] < settings.Inf[i]) {
						lastTemp[i] = settings.Inf[i] + random(1,6)*0.1;
					}
				
				sumADC[i] = 0;
				
				#if defined(SERIAL_DEBUG_TEMP)
				Serial.print(lastTemp[i],1);
				#elif defined(SERIAL_DEBUG_VOLT)
				Serial.print(lastVolt[i],3);
				#elif defined(SERIAL_DEBUG_OHM)
				Serial.print(lastOhm[i],1);
				#endif
				
				#if defined(SERIAL_DEBUG_TEMP) || defined(SERIAL_DEBUG_VOLT) || defined(SERIAL_DEBUG_OHM)
				Serial.print(",\t");
				#endif
				
			}
			
			flags |= FLAG_AVAILABLE_TEMP | FLAG_AVAILABLE_VOLTAGE | FLAG_AVAILABLE_RESISTANCE;//B00011100;
			
			#if defined(SERIAL_DEBUG_TEMP) || defined(SERIAL_DEBUG_VOLT) || defined(SERIAL_DEBUG_OHM)
			Serial.println(flags, BIN);
			#endif	
			
			count=0;
		} else {
			static byte j=0;
			//for(byte j=0; j<settings.n; j+)
			if( j<settings.n ) {
				sumADC[j] += ReadChannel(j);
				j++;
			} else {
				j=0;
				count++;
				
				#ifdef SERIAL_DEBUG_ADC
				for(int i=0; i<settings.n; i++) {
					Serial.print(sumADC[i]);
					Serial.print(", ");
				}
				Serial.println();
				#endif
			}
		}	
	
		if( flags & FLAG_SAVE_SETTINGS )
		{
			StoreEEPROM();
			flags &= ~FLAG_SAVE_SETTINGS;
		}
	}
}


byte TempSensor::CalibrateChannel(CalibrationData data)
{
	//if( data == NULL) return false;
	float t=data.temperature;
	byte c=data.channel;
	if( (flags & FLAG_AVAILABLE_VOLTAGE) && lastVolt[c] > 0 ) {
		ampere[c] = lastVolt[c]*4*B / (4*B*R0 + R0*(4*square(B)*square(t) + 4*B*t*A));
		flags |= FLAG_SAVE_SETTINGS;
		return true;
	} else return false;
}

void TempSensor::ResetSettings()
{
	settings = Settings(16);//Settings::Settings(16);
	StoreEEPROM();
}

void TempSensor::ResetCalibrations()
{
	ampere = I();
	StoreEEPROM();
}

int* TempSensor::ReadAll(byte &n)
{
	n = settings.n;
	int* values = new int[n];
	for(byte i=0; i<n; i++)
		values[i] = ReadChannel((byte)i);
	
	return values;
}

byte TempSensor::GetLastTemp(float lastReadReturn[], byte &n)
{
	if( flags & FLAG_AVAILABLE_TEMP) {
		//float lastReadReturn[16];
		n = settings.n;
		for(byte i=0; i<n; lastReadReturn[i] = lastTemp[i], i++);
		flags &= B11111011;
	} else {
		n=0;
	}
	return n;
}

byte TempSensor::GetLastVolt(float lastReadReturn[], byte &n)
{
	if( flags & B00001000) {
		//float lastReadReturn[16];
		n = settings.n;
		for(byte i=0; i<n; lastReadReturn[i] = lastVolt[i], i++);
		flags &= B11110111;
	} else {
		n=0;
	}
	return n;
}

byte TempSensor::GetLastOhm(float lastReadReturn[], byte &n)
{
	if( flags & B00010000) {
		//float lastReadReturn[16];
		n = settings.n;
		for(byte i=0; i<n; lastReadReturn[i] = lastOhm[i], i++);
		flags &= B11101111;
	} else {
		n=0;
	}
	return n;
}

void TempSensor::LoadEEPROM()
{
	int address = EEPROM_START;
	EEPROM.get(address, settings);
	address += sizeof(settings);
	EEPROM.get(address, ampere);
}

void TempSensor::StoreEEPROM()
{
	int address = EEPROM_START;
	EEPROM.put(address, settings);
	address += sizeof(settings);
	EEPROM.put(address, ampere);
}
