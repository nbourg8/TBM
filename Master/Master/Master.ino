/*
* FubarinoSD_lcd.ino
*
* Created: 12/31/2016 1:33:20 PM
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
/*------------->	TO CONTROLLER
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
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10,9,8,7);

#define pin_menuUP 20
#define pin_Right 19
#define pin_Left 18

const int chipSelect_SD_default = 27; //SD pin chip select for FubarinoSD

int mainMenu = 0;
float bat_main = 0;
float bat_secondary = 0;
int max_menu_count = 6;
int speed = 0;
int spin = 0;
int vert_angle = 0;
int horiz_angle = 0;

int millis_past = millis();
int millis_current = millis_past;

int mySerialData[11];

void setup()
{
	pinMode(pin_menuUP,INPUT);
	pinMode(pin_Right,INPUT);
	pinMode(pin_Left,INPUT);
	
	Serial.begin(115200);	// USB Comm Port
	//Serial0.begin(115200);// UART1 - pins 8 (RX) and 9 (TX)
	Serial1.begin(115200);  // UART2 - pins 28 (RX) and 29 (TX)
	
	pinMode(chipSelect_SD_default, OUTPUT);
	digitalWrite(chipSelect_SD_default,HIGH);
	delay(50); // not required, placed to ensure time for digitalWrite
	if (!SD.begin(chipSelect_SD_default)) // Check if SD is ready
	{
		Serial.println("Card failed, or not present");
		// don't do anything more:
		return;
	}
	Serial.println("card initialized.");
	lcd.begin(16, 2);		// set up LCD with number of columns and rows:
	lcd.clear();
	lcd.home();
	lcd.print("Connecting...");
}


void loop()
{
	
	millis_current = millis();
	if (Serial1.available() >=10)	//ensure Serial communication is functional - don't waste time if remote is sleeping
	{
		if (Serial1.read() == 0x99)	//look for remote address start byte
		{
			for (int i = 1; i<10; i++)	//gather all bytes for later processing
			{
				mySerialData[i] = Serial1.read();
				Serial.print(mySerialData[i], HEX);
				Serial.print(',');
				speed = mySerialData[7];
				spin = mySerialData[8];
				vert_angle = mySerialData[9];
				horiz_angle = mySerialData[10];
			}
			Serial.println();
		}
	}
	if (digitalRead(pin_menuUP) == HIGH)
	{
		mainMenu++;
		Serial.print(mainMenu);
		delay(400);
	}
	if (digitalRead(pin_Left) == HIGH)
	{
		editParam("left");
		delay(300);
	}
	if (digitalRead(pin_Right) == HIGH)
	{
		editParam("right");
		delay(300);
	} //end Detect Button Press
	
	// Handle Menu Rollover
	if (mainMenu < 0)
	{
		mainMenu = max_menu_count;
	}
	else if (mainMenu > max_menu_count)
	{
		mainMenu = 0;
	}//End Handle Menu Rollover
	
	//Show proper menu on LCD
	if (millis_current - millis_past > 250)
	{
		millis_past = millis_current;
		if (mainMenu == 0)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("%      Bat Main");
			lcd.setCursor(1,0);
			lcd.print(bat_main);
			lcd.setCursor(0,1);
			lcd.print("%      Bat 2nd");
			lcd.setCursor(1,1);
			lcd.print(bat_secondary);
		}
		else if (mainMenu == 1)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Save to SD?");
			lcd.setCursor(0,1);
			lcd.print("L=No, R=YES");
		}
		else if (mainMenu == 2)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Factory Reset?");
			lcd.setCursor(0,1);
			lcd.print("L=No, R=YES");
		}
		else if (mainMenu == 3)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Speed:      MPH");
			lcd.setCursor(0,1);
			lcd.print("0  to 100 MPH");
			lcd.setCursor(7,0);
			lcd.print(speed);
		}
		else if (mainMenu == 4)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Spin Amt. %");
			lcd.setCursor(0,1);
			lcd.print("(-50% to +50%)");
			lcd.setCursor(11,0);
			lcd.print(spin);
		}
		else if (mainMenu == 5)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Vert Angle   deg");
			lcd.setCursor(0,1);
			lcd.print("(0 to 90)");
			lcd.setCursor(11,0);
			lcd.print(vert_angle);
		}
		else if (mainMenu == 6)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Hor. Angle   deg");
			lcd.setCursor(0,1);
			lcd.print("(-30 to +30)");
			lcd.setCursor(11,0);
			lcd.print(horiz_angle);
		}//End Show proper menu on LCD
	}
}

void editParam(String direction)
{
	if (mainMenu == 1)
	{
		if (direction == "right")
		{
			SD_Write('Settings: ' + String(mySerialData[7]) + ',' + String(mySerialData[8]) + ',' + String(mySerialData[9]) + ',' + String(mySerialData[10]));
			lcd.clear();
			lcd.home();
			lcd.print("Saved to SD");
			for (int i = 3;i--;i>0)
			{
				lcd.setCursor(0,1);
				lcd.print(i);
				delay(1000);
			}
			mainMenu = 0;
		}
		else
		{
			mainMenu = 0;
		}
	}
	else if (mainMenu == 2)
	{
		//Factory Reset
		speed = 0;
		spin = 0;
		vert_angle = 0;
		horiz_angle = 0;
		//confirm saved
	}
	else if (mainMenu == 3)
	{
		if (direction == "right" && speed < 255)
		{
			speed = speed + 1;
		}
		if (direction == "left" && speed > 0)
		{
			speed = speed - 1;
		}
	}
	else if (mainMenu == 4)
	{
		if (direction == "right" && spin < 50)
		{
			spin = spin + 1;
		}
		else if (direction == "left" && spin > -50)
		{
			spin = spin - 1;
		}
	}
	else if (mainMenu == 5)
	{
		if (direction == "right" && vert_angle < 90)
		{
			vert_angle = vert_angle + 1;
		}
		else if (direction == "left" && vert_angle > 0)
		{
			vert_angle = vert_angle - 1;
		}
		
	}
	else if (mainMenu == 6)
	{
		if (direction == "right" && horiz_angle < 30)
		{
			horiz_angle = horiz_angle + 1;
		}
		else if (direction == "left" && horiz_angle > -30)
		{
			horiz_angle = horiz_angle - 1;
		}
	}
}

void SD_Write(String text)
{
	File dataFile = SD.open("datalog.txt",FILE_WRITE);
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