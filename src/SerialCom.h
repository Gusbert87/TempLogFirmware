/**** SERIAL COMMUNICATION INTERFACE
* Handle the serial communication between arduino and the PC software
*
* TODO: write down sending functions
****/

#ifndef __SERIAL_COM
#define __SERIAL_COM

#include <Arduino.h>
#include "Settings.h"

#define BAUD_RATE 9600

#define START_MARKER_BYTE 		254
#define END_MARKER_BYTE 		255
#define START_MARKER_CHAR 		'<'
#define END_MARKER_CHAR 		'>'
#define SEPARATOR_MARKER_CHAR	','

#define START_MARKER 		START_MARKER_CHAR
#define END_MARKER			END_MARKER_CHAR
#define SEPARATOR_MARKER	SEPARATOR_MARKER_CHAR
#define SPECIAL_BYTE 		253
#define MAX_MESSAGE 		300

#define FLAG_INIT_OK				B00000001
#define FLAG_DATA_AVAILABLE			B00000010
#define FLAG_SETTINGS_REQUEST		B00000100
#define FLAG_TEMPERATURE_REQUEST	B00001000
#define FLAG_SETTINGS_ARRIVED		B00010000
#define FLAG_CALIBRATION_ARRIVED	B00100000
#define FLAG_REPORT_REQUEST			B01000000

#define OUTPUT_START		START_MARKER
#define OUTPUT_END			END_MARKER
#define OUTPUT_SEPARATOR	SEPARATOR_MARKER
#define OUTPUT_NODATA		"NODATA"
#define OUTPUT_READY		"READY"
#define OUTPUT_OK			"OK"
#define OUTPUT_TEMPERATURE	't'
#define OUTPUT_SETTINGS		's'
#define OUTPUT_TRUE			'T'
#define OUTPUT_FALSE		'F'

#define INPUT_START					START_MARKER
#define INPUT_END					END_MARKER
#define INPUT_SEPARATOR				SEPARATOR_MARKER
#define INPUT_REPORT_REQUEST		'R'
#define INPUT_TEMPERATURE_REQUEST	'T'
#define INPUT_SETTINGS_REQUEST		'S'
#define INPUT_SETTINGS_ARRIVED		's'
#define INPUT_CALIBRATION_ARRIVED	'c'

#define FLAG_SHIFT					2

//#define SERIAL_DEBUG

class SerialCom {
	public:
	
		SerialCom();										//initialise the properties
		void Init();										//init the serial and send the READY message
		
		byte Execute();										//execute the main loop
		
		Settings GetAvaiableSettings();						//return the settings recieved from serial
		CalibrationData GetCalibration();					//return the calibration data recieved from serial
		
		void SendSettings(Settings set);					//send a message with the settings after a settings request
		void SendTemperature(float temp[], byte length);	//send a message with the temperatures after a request
		void SendNoTemperatureData();
		void SendNoSettingsData();
		void SendNoData();									//data not available message
		void SendOk();										//data recieved and handled
		
		void SendMonitorReport(Settings set, I ampere);
		
	private:
	
		void SendNoData(char type);			//data not avaiable message. type define the type of data not available
	
		void GetData();						//check for avaiable data on serial port and read the data
		void SendData();
		
		void ParseData();					//parse the data recieved from GetData, and turn on the appropriate flag
		
		byte FlagChecker();					//return the right command to the base program
		
		char inputChars[MAX_MESSAGE];
		char outputChars[MAX_MESSAGE+2];
		
		Settings settings;					//a temporary SerialCom to TempSensor settings interface
		CalibrationData calibrationData;	//a temporary SerialCom to TempSensor calibration interface
		
		byte flags;							//B0MCSTRNI		I = init function called
											//				N = new data available
											//				R = settings request arrived
											//				T = temperature request arrived
											//				S = settings arrived
											//				C = calibration arrived
											//				M = report request arrived
											
};

#endif //__SERIAL_COM
