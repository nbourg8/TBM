/*
* TestBoard2.ino
*
* Created: 1/5/2017 6:27:03 AM
* Author: nbourg8
*/

//	COMMUNICATION PROTOCOL (CUSTOM)
/*------------->	FROM CONTROLLER
BYTE		DATA		DESCRIPTION														EXAMPLE
0			0x99		Start byte - Indicates beginning of frame						serial.write(0x99); //start byte indicator of incoming data
1			0x02		MSB Main Bat Voltage											serial.write(0x02); //
2			0x3E		LSB Main Bat Voltage											serial.write(0x3E); //0x02 + 0x3E = 0x23E = 574 decimal = 3.7volts
3			0x02		MSB 2nd Bat Voltage												serial.write(0x02); //
4			0x3A		LSB 2nd Bat Voltage												serial.write(0x3A); //0x02 + 0x3A = 0x23A = 570 decimal = 3.67volts
5			0x01		Save to SD Indicator											serial.write(0x01); //(Typically 0 or 1) Save to SD set to TRUE
6			0x00		Factory Reset Indicator											serial.write((byte)00); //(Typically 0 or 1) Factory Reset set to FALSE
7			0x60		Speed															serial.write(0x60); // 0x60 = 96dec = 96MPH
8			0x0A		Spin															serial.write(0x0A); //0x0A = 10dec = 10%
9			0x3C		Vertical Angle													serial.write(0x3C); //0x3C = 60dec = 60 degrees
10			0x1E		Horizontal Angle												serial.write(0x1E); //(0 to 60dec is mapped from -30 to +30)  0x1E = 30dec or 0 degrees after being mapped
*/


#include <SD.h>

#define btn 12
#define cardDetect 7
#define vBat 9
const int chipSelect_SD_default = 4; //SD pin chip select for FubarinoSD


void setup()
{
	Serial1.begin(115200);
	pinMode(btn,INPUT);
	pinMode(cardDetect,INPUT_PULLUP);
	pinMode(chipSelect_SD_default, OUTPUT);
	digitalWrite(chipSelect_SD_default,HIGH);
	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect_SD_default))
	{
		Serial.println("Card failed, or not present");
		// don't do anything more:
		return;
	}
}

void loop()
{
	if (digitalRead(btn)==HIGH)
	{
		Serial1.write(0x99);
		Serial1.write(0x02);
		Serial1.write(0x3E);
		Serial1.write(0x02);
		Serial1.write(0x3A);
		Serial1.write(0x01);
		Serial1.write((byte)00);
		Serial1.write(0x60);
		Serial1.write(0x0A);
		Serial1.write(0x3C);
		Serial1.write(0x1E);
		//float measuredvbat = analogRead(vBat);
		//measuredvbat *= 2;
		//measuredvbat *= 3.3;
		//measuredvbat /= 1024;
		//Serial.println(measuredvbat);
		//SD_Write("Battery Voltage: " + String(measuredvbat));
	}
	if (Serial1.available()>0)
	{
		Serial.write(Serial1.read());
	}

	delay(50);
}

void SD_Write(String text)
{
	File dataFile = SD.open("datalog.txt",FILE_WRITE);
	if (digitalRead(cardDetect)==HIGH)
	{
		if (dataFile)
		{
			dataFile.println(text);
			dataFile.close();

			Serial.println(text);
		}
		else
		{
			Serial.println("error opening file");
		}
	}
	
}