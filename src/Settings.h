#ifndef __SETTINGS
#define __SETTINGS
 
/**** STRUCT SETTINGS
	Contain the basic settings for the array of sensors

	TODO: add multiple threshold for channels
****/
#include <Arduino.h>

#define BYTE_TRUE(a,b)		a&(1<<b)
#define BYTE_FALSE(a,b)		!BYTE_TRUE(a,b)
#define SET_BYTE(a,b)		a|=(1<<b)
#define CLEAR_BYTE(a,b)		a&=(!(1<<b))

struct Settings {
	byte n;               //number of channels
	word isSup;           //every bit say if the upper threshold is active for the given channel
	word isInf;           //every bit say if the lower threshold is active for the given channel
	float Sup[16];        //contain the upper threshold for the given channel
	float Inf[16];        //contain the lower threshold for the given channel
	
	Settings(int in);     //needed for returning a NULL settings when TempSensor is not initialized
	Settings();
	
	bool operator== (int r);
	bool operator!= (int r);
};

//typedef float I[16];      

struct I {
	float i[16];														//containt the current flowing in the RTD
	
	I();																//constructor for auto initialization of non zero current
	I(float ampere);
	float& operator[](unsigned int idx)	{return i[idx];}				//indexing operator
	const float& operator[](unsigned int idx) const	{return i[idx];}	//indexing operator
};

struct CalibrationData {//data relative to the calibration of one channel
	byte channel;		//the channel to calibrate
	float temperature;	//the reference temperature
	
	//The following functions interact with CalibrationData as the channels are numbered starting from 1
	//so if you want to init the number of channel to zero you have to call CalibrationData(1)
	//the same for the operator== and operator!=.
	//For these functions any value outside the 1-16 inclusive range act as a non-valid object
	CalibrationData();
	CalibrationData(int in);
	
	bool operator==(int r);
	bool operator!=(int r);
};

#endif //__SETTINGS