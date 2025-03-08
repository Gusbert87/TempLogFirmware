#include <Arduino.h>
#include "TempSensor.h"
#include "SerialCom.h"

//#define RESET_SETTINGS
//#define RESET_CALIBRATIONS

#define COMMAND_SEND_SETTINGS 		FLAG_SETTINGS_REQUEST >> FLAG_SHIFT
#define COMMAND_SEND_LAST_TEMP		FLAG_TEMPERATURE_REQUEST >> FLAG_SHIFT
#define COMMAND_GET_SETTINGS		FLAG_SETTINGS_ARRIVED >> FLAG_SHIFT	
#define COMMAND_GET_CALIBRATION		FLAG_CALIBRATION_ARRIVED >> FLAG_SHIFT
#define COMMAND_SEND_REPORT			FLAG_REPORT_REQUEST >> FLAG_SHIFT

TempSensor tempSensor;
SerialCom serialCom;

byte command;

void setup() {
	tempSensor.Init();
	
	#ifdef RESET_SETTINGS
		tempSensor.ResetSettings();
	#endif
	
	#ifdef RESET_CALIBRATIONS
		tempSensor.ResetCalibrations();
	#endif
	
	serialCom.Init();
}

void loop() {
	command = serialCom.Execute();
	
	if(command & COMMAND_SEND_SETTINGS) {
		Settings temp = tempSensor.GetSettings();
		
		if(tempSensor.isInit())
			serialCom.SendSettings( temp );
		else
			serialCom.SendNoSettingsData();
	}
	
	if(command & COMMAND_SEND_LAST_TEMP) {
		float T[16];
		byte n;
		
		tempSensor.GetLastTemp(T, n);
		
		serialCom.SendTemperature(T, n);
			
	}
	
	if(command & COMMAND_GET_SETTINGS)
	{
		Settings temp = serialCom.GetAvaiableSettings();
		Serial.print("\n");
		Serial.println(temp.n);
		tempSensor.SetSettings(temp);
	}
	
	if(command & COMMAND_GET_CALIBRATION)
	{
		byte ok;
		CalibrationData temp = serialCom.GetCalibration();
		
		ok = tempSensor.CalibrateChannel(temp);
		
		if(ok) serialCom.SendOk();
		else serialCom.SendNoData();
	}
	
	if(command & COMMAND_SEND_REPORT) {
		Settings set = tempSensor.GetSettings();
		I ampere = tempSensor.GetCalibrations();
		
		serialCom.SendMonitorReport(set, ampere);
	}
	
	tempSensor.Execute();
}


