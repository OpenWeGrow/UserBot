/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/*
This code is an example that intends to show the capabilities of UserBot,
while expalinning how to edit this source code to achieve the control you may need
*/

#include <SPI.h>
#include <EEPROM.h>
#include "GroBot_Variables.h"
#include <Wire.h>


#ifdef RF	
	#include "nRF24L01.h"
	#include "RF24.h"		
#endif


#include "ComsTask.h"
#include "SerialTask.h"
#include "SensorsTask.h"

#ifdef RF
	RF24 radio(9,10);
#endif

ComsTask vComsTask;
SerialTask vSerialTask;
SensorsTask vSensorsTask;


void setup(void)
{

    Serial.begin(230400);
	Serial.setTimeout(10);
    Wire.begin();
	#ifdef RF
		vComsTask.vGoComsTask(radio); 	
	#else
		vComsTask.vGoComsTask();
	#endif

	vSerialTask.printDisclamer();

    radio.printDetails();
  
}

void loop(void)
{	
    //If the serial Number is the default one, the module does not communicate and awaits a valid serial number definition

	if(!(serialNumber[4] == DEFAULT_SERIALNUMBER_4 && serialNumber[5] == DEFAULT_SERIALNUMBER_5 && serialNumber[6] == DEFAULT_SERIALNUMBER_6 && serialNumber[7] == DEFAULT_SERIALNUMBER_7))
	{
		#ifdef RF
			vComsTask.vGoComsTask(radio);
		#else
			vComsTask.vGoComsTask();
		#endif	
	}
	else
		Serial.println("RFH");
		
    vSerialTask.vGoSerialTask(radio);		
    vSensorsTask.GoSensorsTask();
	
}
/****************************************************************************/

