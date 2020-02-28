/*
 Copyright (C) 2020 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/*
This code is an example that intends to show the base capabilities of UserBot,
this example covers the use of UserBot as an 8 output device.
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

unsigned char timerCounter = 0;

void setup(void)
{
	//There are two global arrays to handle all variables data
	//inputs[]/outputs[]
	//Each array has a max of 10 positions
	//For each sensor or output the user must define:
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
	
	inputs[INPUT_INDEX0].type = OPEN_DEFAULT;   //Unused Inpu
	inputs[INPUT_INDEX1].type = OPEN_DEFAULT;   //Unused Inpu
	inputs[INPUT_INDEX2].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX3].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX4].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX5].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX6].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX7].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX8].type = OPEN_DEFAULT;	//Unused Input
	inputs[INPUT_INDEX9].type = OPEN_DEFAULT;	//Unused Input
	
	outputs[OUTPUT_INDEX0].arduinoPin = 3;  //Output 1
	outputs[OUTPUT_INDEX1].arduinoPin = 4;	//Output 2
	outputs[OUTPUT_INDEX2].arduinoPin = 5;	//Output 3
	outputs[OUTPUT_INDEX3].arduinoPin = 6;	//Output 4
	outputs[OUTPUT_INDEX4].arduinoPin = 7;	//Output 5
	outputs[OUTPUT_INDEX5].arduinoPin = 8;	//Output 6
	outputs[OUTPUT_INDEX6].arduinoPin = A0;	//Output 7
	outputs[OUTPUT_INDEX7].arduinoPin = A1;	//Output 8
	outputs[OUTPUT_INDEX8].arduinoPin = 0;	//Unused Output
	outputs[OUTPUT_INDEX9].arduinoPin = 0;	//Unused Output
	
	outputs[OUTPUT_INDEX0].type = IO_NOT_CONNECTED; //Output 1 Not Connected by default
	outputs[OUTPUT_INDEX1].type = IO_NOT_CONNECTED;	//Output 2 Not Connected by default
	outputs[OUTPUT_INDEX2].type = IO_NOT_CONNECTED;	//Output 3 Not Connected by default
	outputs[OUTPUT_INDEX3].type = IO_NOT_CONNECTED; //Output 4 Not Connected by default
	outputs[OUTPUT_INDEX4].type = IO_NOT_CONNECTED;	//Output 5 Not Connected by default
	outputs[OUTPUT_INDEX5].type = IO_NOT_CONNECTED;	//Output 6 Not Connected by default
	outputs[OUTPUT_INDEX6].type = IO_NOT_CONNECTED;	//Output 7 Not Connected by default
	outputs[OUTPUT_INDEX7].type = IO_NOT_CONNECTED;	//Output 8 Not Connected by default
	outputs[OUTPUT_INDEX8].type = OPEN_DEFAULT;	//Unused Output
	outputs[OUTPUT_INDEX9].type = OPEN_DEFAULT;	//Unused Output

    //Uncomment this line when your ON is LOW and OFF is HIGH
    //vComsTask.vSetReversedOutputs(true);

    if(reversedOutputs)
    {
        digitalWrite(outputs[OUTPUT_INDEX0].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX1].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX2].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX3].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX4].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX5].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX6].arduinoPin, HIGH);
        digitalWrite(outputs[OUTPUT_INDEX7].arduinoPin, HIGH);

        outputs[OUTPUT_INDEX0].value = 255;
        outputs[OUTPUT_INDEX1].value = 255;    
        outputs[OUTPUT_INDEX2].value = 255;
        outputs[OUTPUT_INDEX3].value = 255; 
        outputs[OUTPUT_INDEX4].value = 255;
        outputs[OUTPUT_INDEX5].value = 255; 
        outputs[OUTPUT_INDEX6].value = 255;
        outputs[OUTPUT_INDEX7].value = 255; 

    }  

    //***********************  IO Config  *****************************//
    /*Here you need to set your used pins as inputs or outputs*/
    //pinMode(inputs[INPUT_INDEX0].arduinoPin, INPUT);
    //pinMode(inputs[INPUT_INDEX1].arduinoPin, INPUT); 
      
    pinMode(outputs[OUTPUT_INDEX0].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX1].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX2].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX3].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX4].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX5].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX6].arduinoPin, OUTPUT);	
    pinMode(outputs[OUTPUT_INDEX7].arduinoPin, OUTPUT);	

	Serial.begin(230400);
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
	/*else
	{
		//If no serial number is defined, to module gets stuck here until a valid serial number is defined
		Serial.println("RFH");
	}*/

	//UART Task
	vSerialTask.vGoSerialTask(radio);
	//IO Management Task
	vSensorsTask.GoSensorsTask();
}

//Timer Interrupt Handler
SIGNAL(TIMER1_COMPA_vect)
{
	timerCounter++;

	if(timerCounter == TIME_2_PRINT_DEBUG)
	{
		timerCounter=0;

		//If the serial Number is the default one, the module does not communicate and awaits a valid serial number definition
		if((serialNumber[4] == DEFAULT_SERIALNUMBER_4 && serialNumber[5] == DEFAULT_SERIALNUMBER_5 && serialNumber[6] == DEFAULT_SERIALNUMBER_6 && serialNumber[7] == DEFAULT_SERIALNUMBER_7))
		{	
			//RF Task
			Serial.println("RFH");
			Serial.println("");
		}

		//Dump output states in the UART		
        vSensorsTask.printOutputState(OUTPUT_INDEX0);
        vSensorsTask.printOutputState(OUTPUT_INDEX1);
        vSensorsTask.printOutputState(OUTPUT_INDEX2);
        vSensorsTask.printOutputState(OUTPUT_INDEX3);
        vSensorsTask.printOutputState(OUTPUT_INDEX4);
        vSensorsTask.printOutputState(OUTPUT_INDEX5);
        vSensorsTask.printOutputState(OUTPUT_INDEX6);
        vSensorsTask.printOutputState(OUTPUT_INDEX7);

       
        
	}
}
/****************************************************************************/