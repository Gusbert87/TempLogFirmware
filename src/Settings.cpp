#include "Settings.h"

Settings::Settings()
{
	n=0; isSup=0; isInf=0;
	for(byte i=0; i<16; Sup[i]=0, Inf[i]=0,i++);
}

Settings::Settings(int in)
{
	if(in<=0 || n>16) { 
		n=0; isSup=0; isInf=0;
		for(byte i=0; i<16; Sup[i]=0, Inf[i]=0,i++);
	} else {
		n=in; isSup=0; isInf=0;
		for(byte i=0; i<16; Sup[i]=0, Inf[i]=0,i++);
	}
}

bool Settings::operator==(int r)
{
	return (n == r);
}

bool Settings::operator!=(int r)
{
	return (n != r);
}

I::I()
{
	for(int j=0; j<16; j++) i[j]=0.01;
}

I::I(float ampere)
{
	for(int j=0; j<16; j++) i[j]=ampere;
}

CalibrationData::CalibrationData(int in)
{
	if(in <=0 || in > 16)
		channel = B11111111;
	else
		channel = in-1;
}

CalibrationData::CalibrationData()
{
	channel = B11111111;
}

bool CalibrationData::operator==(int r)
{
	if(r<=0 || r>16) {
		if(channel == B11111111) return true;
		else return false;
	} else {
		return channel==(r-1);
	}
}

bool CalibrationData::operator!=(int r)
{
	if(r<=0 || r>16) {
		if(channel != B11111111) return true;
		else return false;
	} else {
		return channel!=(r-1);
	}
}