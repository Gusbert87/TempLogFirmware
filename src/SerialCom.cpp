#include "SerialCom.h"

SerialCom::SerialCom() 
{
	flags = B00000000;
}

void SerialCom::Init() 
{
	Serial.begin(BAUD_RATE);
	Serial.write(OUTPUT_START);
	Serial.write(OUTPUT_READY);
	Serial.write(OUTPUT_END);
	flags = FLAG_INIT_OK;
}

byte SerialCom::Execute() 
{
	if(flags & FLAG_INIT_OK) {
		GetData();
		
		if(flags & FLAG_DATA_AVAILABLE) {
			ParseData();
		}
	}
	
	return (flags >> FLAG_SHIFT);
}

void SerialCom::GetData() 
{
    static bool inProgress = false;
    static byte ndx = 0;
    char singleByte;

    while (Serial.available() > 0 && !(flags & FLAG_DATA_AVAILABLE) ) {
        singleByte = Serial.read();

		if (inProgress == true) {
			if (singleByte != INPUT_END) {
				inputChars[ndx] = singleByte;
				ndx++;
				if (ndx >= MAX_MESSAGE) 
					ndx = MAX_MESSAGE - 1;
			} else {
				inputChars[ndx] = '\0';
				inProgress = false;
				ndx = 0;
				flags |= FLAG_DATA_AVAILABLE;
				#ifdef SERIAL_DEBUG
					Serial.print("message recieved: ");
					Serial.println(inputChars);
				#endif
			}
		} else if (singleByte == INPUT_START) {
			inProgress = true;
		}
    }
}

void SerialCom::ParseData() 
{
	if( inputChars[0] == INPUT_REPORT_REQUEST) {
		flags |= FLAG_REPORT_REQUEST;
	} else if( inputChars[0] == INPUT_SETTINGS_REQUEST) {
		flags |= FLAG_SETTINGS_REQUEST;
	} else if( inputChars[0] == INPUT_TEMPERATURE_REQUEST) {
		flags |= FLAG_TEMPERATURE_REQUEST;
	} else if( inputChars[0] == INPUT_SETTINGS_ARRIVED) {
    const char separator[] = {INPUT_SEPARATOR, '\0'};
		char *temp;
		byte i;
		//Serial.println(inputChars+1);
		temp = strtok( inputChars + 1, separator);
    	//Serial.println(temp);
    	//Serial.println(atoi(temp));
		settings.n = atoi(temp);
		
		if(settings.n>16) settings.n=0;
		
		settings.isSup = 0;
		for(i=0; i<settings.n; i++) {
			temp = strtok(NULL, separator);
			if(temp[0] == 'T') settings.isSup |= (1<<i);
		}
		
		settings.isInf = 0;
		for(i=0; i<settings.n; i++) {
			temp = strtok(NULL, separator);
			if(temp[0] == 'T') settings.isInf |= (1<<i);
		}
		
		for(i=0; i<settings.n; i++) {
			temp = strtok(NULL, separator);
			settings.Sup[i] = atof(temp);
		}
		
		for(i=0; i<settings.n; i++) {
			temp = strtok(NULL, separator);
			settings.Inf[i] = atof(temp);
		}
		
		flags |= FLAG_SETTINGS_ARRIVED;
	} else if( inputChars[0] == INPUT_CALIBRATION_ARRIVED) {
    const char separator[] = {INPUT_SEPARATOR, '\0'};
		char *temp;
		
		temp = strtok( inputChars + 1, separator );
		
		calibrationData.channel = atoi(temp);
		if(calibrationData.channel > 16) calibrationData.channel = B11111111;
		
		temp = strtok( NULL, separator);
		calibrationData.temperature = atof(temp);
		
		flags |= FLAG_CALIBRATION_ARRIVED;
	}
	
	flags &= ~FLAG_DATA_AVAILABLE;//B11111101;
	#ifdef SERIAL_DEBUG
	Serial.print("flags= ");
	Serial.println(flags, BIN);
	#endif
}

Settings SerialCom::GetAvaiableSettings()
{
	if(flags & (FLAG_SETTINGS_ARRIVED | FLAG_INIT_OK)) //{
		flags &= B11101111;

	return settings;
	//} else return NULL;
}

CalibrationData SerialCom::GetCalibration()
{
	if(flags & (FLAG_CALIBRATION_ARRIVED | FLAG_INIT_OK)) //{
		flags &= B11011111;
		
	return calibrationData;
	//} else return NULL;
}

void SerialCom::SendTemperature(float temp[], byte length)
{
	if(flags & (FLAG_TEMPERATURE_REQUEST | FLAG_INIT_OK) && length != 0) {
		int count=0;
		char buff[7];
		
		outputChars[count++] = OUTPUT_START;
		//count+=2;										//skip the length char for futher calculations
		outputChars[count++] = OUTPUT_TEMPERATURE;
		outputChars[count++] = OUTPUT_SEPARATOR;
		itoa(length, buff, 10);
		outputChars[count++] = buff[0];
		if(length>9) outputChars[count++] = buff[1];
		outputChars[count++] = OUTPUT_SEPARATOR;
		
		for(int i=0; i<length; i++) {
			dtostrf(temp[i], 3, 1, buff);
			
			//Serial.println(buff);
			
			for(int j=0; buff[j]!= '\0'; j++) 
				outputChars[count++] = buff[j];
				
			outputChars[count++] = OUTPUT_SEPARATOR;
		}
		
		outputChars[count-1] = OUTPUT_END;
		outputChars[count] = '\0';
		//outputChars[1] = strlen(outputChars) - 3;
		
		SendData();
	} else {
		SendNoTemperatureData();
	}
	flags &= B11110111;
}

void SerialCom::SendSettings(Settings set)
{
	if(flags & ( FLAG_SETTINGS_REQUEST | FLAG_INIT_OK )) {
		int count=0, i=0;
		char buff[7];
		
		outputChars[count++] = OUTPUT_START;
		outputChars[count++] = OUTPUT_SETTINGS;
		outputChars[count++] = OUTPUT_SEPARATOR;
		
		itoa(set.n, buff, 10);
		outputChars[count++] = buff[0];
		if(set.n>9) outputChars[count++] = buff[1];
		
		outputChars[count++] = OUTPUT_SEPARATOR;
		for(i=0; i<set.n; i++) {
			if(set.isSup & (1<<i)) outputChars[count++] = OUTPUT_TRUE;
			else outputChars[count++] = OUTPUT_FALSE;
			outputChars[count++] = OUTPUT_SEPARATOR;
		}
		
		for(i=0; i<set.n; i++) {
			if(set.isInf & (1<<i)) outputChars[count++] = OUTPUT_TRUE;
			else outputChars[count++] = OUTPUT_FALSE;
			outputChars[count++] = OUTPUT_SEPARATOR;
		}
		
		for(i=0; i<set.n; i++) {
			dtostrf(set.Sup[i], 3, 1, buff);
			
			for(int j=0; buff[j]!= '\0'; j++) 
				outputChars[count++] = buff[j];
				
			outputChars[count++] = OUTPUT_SEPARATOR;
		}
		
		for(i=0; i<set.n; i++) {
			dtostrf(set.Inf[i], 3, 1, buff);
			
			for(int j=0; buff[j]!= '\0'; j++) 
				outputChars[count++] = buff[j];
				
			outputChars[count++] = OUTPUT_SEPARATOR;
		}
		
		outputChars[count-1] = OUTPUT_END;
		outputChars[count] = '\0';
		
		//outputChars[1] = strlen(outputChars) - 3;
		
		SendData();
	} else {
		SendNoSettingsData();
	} 
	flags &= B11111011;
}

void SerialCom::SendMonitorReport( Settings set, I ampere)
{
	if( flags & (FLAG_REPORT_REQUEST | FLAG_INIT_OK)) {
		int i;
		Serial.println("Report:");  
				
		Serial.print("Numero canali: ");
		Serial.println((short int)set.n);
		
		Serial.println("Soglia superiore attiva:");
		for(i=0;i<set.n;i++) {
			Serial.print("  canale ");
			Serial.print(i);
			Serial.print(":\t");
			if(set.isSup & (1<<i))	Serial.println("SI");
			else					Serial.println("NO");
		}
		
		Serial.println("Soglia inferiore attiva:");
		for(i=0;i<set.n;i++) {
			Serial.print("  canale ");
			Serial.print(i);
			Serial.print(":\t");
			if(set.isInf & (1<<i))	Serial.println("SI");
			else 					Serial.println("NO");
		}
		
		Serial.println("Soglia superiore:");
		for(i=0;i<set.n;i++) {
			Serial.print("  canale ");
			Serial.print(i);
			Serial.print(":\t");
			Serial.println(set.Sup[i]);
		}
		
		Serial.println("Soglia inferiore:");
		for(i=0;i<set.n;i++) {
			Serial.print("  canale ");
			Serial.print(i);
			Serial.print(":\t");
			Serial.println(set.Inf[i]);
		}
		
		Serial.println("Corrente:");
		for(i=0;i<set.n;i++) {
			Serial.print("  canale ");
			Serial.print(i);
			Serial.print(":\t");
			Serial.println(ampere[i],6);
		}
		flags &= B10111111;
	}
}

void SerialCom::SendNoTemperatureData() {
	SendNoData(OUTPUT_TEMPERATURE);
}
void SerialCom::SendNoSettingsData(){
	SendNoData(OUTPUT_SETTINGS);
}

void SerialCom::SendNoData()
{
	SendNoData(0);
}

void SerialCom::SendNoData(char type) {
	char buffer[11];
	int idx = 0;
	
	buffer[idx++] = OUTPUT_START;
	if(type != 0) {
		buffer[idx++] = type;
		buffer[idx++] = OUTPUT_SEPARATOR;
	}
	
	strcpy( buffer + idx, OUTPUT_NODATA);
	
	Serial.write(buffer);
	Serial.write(OUTPUT_END);
}

void SerialCom::SendOk()
{
	Serial.write(OUTPUT_START);
	Serial.write(OUTPUT_OK);
	Serial.write(OUTPUT_END);
}

void SerialCom::SendData()
{
	Serial.write(outputChars);
}
