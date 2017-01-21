/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Fubarino SD (1.5), Platform=pic32, Package=chipKIT
*/

#define __PIC32_32MX795F512H__
#define ARDUINO 106012
#define ARDUINO_MAIN
#define F_CPU 80000000L
#define printf iprintf
#define __PIC32__
#define ARDUINO_ARCH_PIC32
#define F_CPU 80000000L
#define _BOARD_FUBARINO_SD_
#define MPIDEVER 16777998
#define _USE_USB_FOR_SERIAL_

//
//
void editParam(String direction);
void SD_Write(String text);

#include "Board_Defs.h" 
#include "arduino.h"
#include "Master.ino"
