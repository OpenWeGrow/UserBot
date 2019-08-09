/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/*
This code is an example that intends to show the base capabilities of UserBot,
this example covers the usage of a peristaltic pump acting on an output and setting its desirable "speed/intensity" using one of the PWM pins of Arduino.
The code is based on Non-Preemptive Multitasking, so you can add any more tasks you might need, just be sure not to exceed CPU time and to use non-blocking code.
*/

#include <SPI.h>
#include <EEPROM.h>
#include "GroBot_Variables.h"
#include <Wire.h>


#include "nRF24L01.h"
#include "RF24.h"		



#include "ComsTask.h"
#include "SerialTask.h"
#include "SensorsTask.h"

#define TIME_2_PRINT_DEBUG 10  //Defines the number a second for a UART output of the state of the sensors

RF24 radio(9,10);  //Pin 9 and 10 are used by the NRF24L01

ComsTask vComsTask;
SerialTask vSerialTask;
SensorsTask vSensorsTask;

unsigned char timerCounter =0;

void setup(void)
{

	//There are two global arrays to handle all variables data
	//inputs[]/outputs[]
	//Each array has a max of 10 positions
	//For each sensor or output the user must define:	// 
	//     
	//     - The pin that is where the sensor/device is connected  - ensure zero(0) if not used
	//     - The type of the sensor/output                         - ensure OPEN_DEFAULT if not used
	//	
	

	inputs[INPUT_INDEX0].arduinoPin = 0;    //Unused Input
	inputs[INPUT_INDEX1].arduinoPin = 0;    //Unused Input
	inputs[INPUT_INDEX2].arduinoPin = 0;    //Unused Input
	inputs[INPUT_INDEX3].arduinoPin = 0;	//Unused Input
	inputs[INPUT_INDEX4].arduinoPin = 0;	//Unused Input
	inputs[INPUT_INDEX5].arduinoPin = 0;	//Unused Input
	inputs[INPUT_INDEX6].arduinoPin = 0;	//Unused Input
	inputs[INPUT_INDEX7].arduinoPin = 0;	//Unused Input
	inputs[INPUT_INDEX8].arduinoPin = 0;	//Unused Input
	inputs[INPUT_INDEX9].arduinoPin = 0;	//Unused Input
	
	inputs[INPUT_INDEX0].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX1].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX2].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX3].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX4].type = OPEN_DEFAULT;	//Unused Input	
	inputs[INPUT_INDEX5].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX6].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX7].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX8].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX9].type = OPEN_DEFAULT;	//Unused Input
	
	outputs[OUTPUT_INDEX0].arduinoPin = 5;  //LED Pin
	outputs[OUTPUT_INDEX1].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX2].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX3].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX4].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX5].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX6].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX7].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX8].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX9].arduinoPin = 0;	//Unused Output
	
	outputs[OUTPUT_INDEX0].type = PERISTALTIC_PUMP;  //DC Motor as peristaltic pump
	outputs[OUTPUT_INDEX1].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX2].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX3].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX4].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX5].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX6].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX7].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX8].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX9].type = OPEN_DEFAULT;	//Unused Output

    Serial.begin(230400);  //
	Serial.setTimeout(10);	

	vComsTask.vGoComsTask(radio); 

	vSerialTask.printDisclamer();

    radio.printDetails();
	
	//Timed Interrupt for debug Output
	//Timer1 interrupt at 1Hz
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same for TCCR1B
	TCNT1  = 0;//initialize counter value to 0
	// set compare match register for 1hz increments
	OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
	// turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// Set CS12 and CS10 bits for 1024 prescaler
	TCCR1B |= (1 << CS12) | (1 << CS10);  
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
  
}

//Main program loop
void loop(void)
{	
    //If the serial Number is the default one, the module does not communicate and awaits a valid serial number definition
	if(!(serialNumber[4] == DEFAULT_SERIALNUMBER_4 && serialNumber[5] == DEFAULT_SERIALNUMBER_5 && serialNumber[6] == DEFAULT_SERIALNUMBER_6 && serialNumber[7] == DEFAULT_SERIALNUMBER_7))
	{	
		//RF Task
		vComsTask.vGoComsTask(radio);	
	}
	else
	{
		//If no serial number is defined, to module get's stuck here until a valid serial number is defined
		Serial.println("RFH");
	}
		
    //UART Task
    vSerialTask.vGoSerialTask(radio);	
	//IO Management Task
    vSensorsTask.GoSensorsTask();
	
}

//Timer Interrupt Handler
SIGNAL(TIMER1_COMPA_vect) 
{
	timerCounter++;
	
    if(timerCounter==TIME_2_PRINT_DEBUG)
    {
		timerCounter=0;
		//Dump sampled data in the UART
	
	    Serial.print(F("DC Motor-"));
		if(outputs[OUTPUT_INDEX0].value==0)
		{
			Serial.print(F("OFF Speed-"));			
		}
		else
		{
			Serial.print(F("ON Speed-"));
		}
		
		Serial.println(outputs[OUTPUT_INDEX0].speed,DEC);
		Serial.println("");
	}
	
}
/****************************************************************************/

