
/**** Temperature Sensor interface
* Handle the ADC reads and conversion in temperature.
* Also handle the threshold behaviour and the calibration of the RDTs
*
* TODO:	add calibration functions and threshold behaviour
*
*****/

#ifndef __TEMP_LOG
#define __TEMP_LOG

#include <Arduino.h>
#include "Settings.h"

//#define SERIAL_DEBUG_ADC	//show every ADC conversion on serial monitor
//#define SERIAL_DEBUG_TEMP	//show every temperature read
//#define SERIAL_DEBUG_VOLT	//show evety voltage read
//#define SERIAL_DEBUG_OHM	//show every resistance value read
//#define SWITCH_TEST

#define TEMP_LOG_VER_2_00

#define AREF 5.030          //ADC reference voltage
#define MAXANALOG 1023      //max value that ADC can read (10 bit)

#define R0 1000.f           //RTD resistance at 0°C
#define A 3.90802e-3        //A coefficient for RTD formula
#define B -5.802e-7         //B coefficient for RTD formula

#define SAMPLES   2000      //numer of samples averaged

#define SWITCH    7         //pin of the corrector switch 

#define EEPROM_START  0     //the starting addres of EEPROM storing

/**** STRUCT SETTINGS REFERENCE:
*    struct Settings {
*    	byte n;               //number of channels
*    	word isSup;           //every bit say if the upper threshold is active for the given channel
*    	word isInf;           //every bit say if the lower threshold is active for the given channel
*    	float Sup[16];        //contain the upper threshold for the given channel
*    	float Inf[16];        //contain the lower threshold for the given channel
*   	float I[16];          //containt the current flowing in the RTD
*  	};
****/

#define FLAG_INIT_OK				B00000001
#define FLAG_SAVE_SETTINGS			B00000010
#define FLAG_AVAILABLE_TEMP			B00000100
#define FLAG_AVAILABLE_VOLTAGE		B00001000
#define FLAG_AVAILABLE_RESISTANCE	B00010000


class TempSensor {
	public:

		TempSensor();                             				//constructor - initialise the variables

		void Init();                              				//inzialise the class loading the settings from EEPROM
    
		void Execute();                           				//execute the main loop

		byte GetLastTemp(float lastReadReturn[], byte &n); 	//return the last temperature measured
		byte GetLastVolt(float lastReadReturn[], byte &n);     //return the last voltage measured
		byte GetLastOhm(float lastReadReturn[], byte &n);      //return the last resistance measured

		Settings GetSettings() 					{return settings;}              			//return the settings from the private property.
		void SetSettings(Settings newSettings) 	{settings = newSettings; StoreEEPROM();}	//set the given settings to the private property and store to eeprom.
		I GetCalibrations()						{return ampere;}							//return the ampere value of the current generators.
		bool isInit()							{return flags & FLAG_INIT_OK;}				//return if the object is inited.
		
		byte CalibrateChannel(CalibrationData data);			//calibrate one given channel
		
		void ResetSettings();									//reset the EEPROM settings to default
		void ResetCalibrations();								//reset the current values

	private:
		void LoadEEPROM();                        	//load the settings from flash and store it in the private property
		void StoreEEPROM();                       	//store the settings private property to the flash
	
		int ReadChannel(byte channel)	{ return analogRead(channel); }		//return the current value read from the given channel
		int* ReadAll(byte &n);                    	//return an array with the value read from all the channels

		byte flags;                               	//B000RVTSI 	I = initialized class flag
													//				S = settings has to be saved
													//				T = available temperature value
													//				V = available voltage value
													//				R = available resistance value
													
		Settings settings;                        	//a temporary program-to-EEPROM interface for the general settings
		I ampere;									//a temporary program-to-EEPROM interface for the current flowing in the RTDs

		unsigned long sumADC[16];                 	//the array where the ADC results are stored
		int count;                                	//the count of how may reads until now;
		
		float lastTemp[16];                       	//stores the average of the last SAMPLES temperature measured in °C
		float lastVolt[16];                       	//stores the average of the last SAMPLES voltage measured in Volt
		float lastOhm[16];                        	//stores the average of the last SAMPLES resistance measured in Ohm
};

#endif //__TEMP_LOG



