/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "GroBot_Variables.h"
#include "SensorsTask.h"
#include <Arduino.h>
#include "SampleMethods.h"
#include "ComsTask.h"

#define DEBUG_IO_VALUES

sensorsMachineState snsState;

#define ArduinoVRef  5

int time= 0;

SensorsTask::SensorsTask(void)
{
	snsState = INIT_SENSORS;

	//***********************  INPUTS  *****************************//
    /*Here you need to set your used pins as inputs or outputs*/
	pinMode(BUTTON_PIN0, INPUT);
    pinMode(BUTTON_PIN1, INPUT);
    pinMode(BUTTON_PIN2, INPUT);
	pinMode(TEMP_PIN, INPUT);	
}

void SensorsTask::GoSensorsTask(void)
{
    //You can create has many states as you would like, just look at SensorsTask.h and change the enum states
    //Make sure you also implement the state on this file
    //This example has a state to initialize, other to get the temperature from an analogue sensor, third one to poll all buttons and a fourth to print the data gathered
	switch(snsState)
    {
		case INIT_SENSORS:
			//Use this state in the machine to initialize any sensor you may need
			snsState = GET_TEMP;
			break;
		case GET_TEMP:
            //Sampling Temperature Sensor
			time = vSampleMethods.getuSAnalogRead(1);
			inputs[TEMP_INDEX].value += calcTemp(time);
			inputs[TEMP_INDEX].value = inputs[TEMP_INDEX].value/2;                        
			snsState = GET_IOS;
			break;		
		case GET_IOS:
            //Polling all buttons
			if(digitalRead(BUTTON_PIN0))
				inputs[BUTTON_INDEX0].value = 255;
			else
				inputs[BUTTON_INDEX0].value = 0x00;

            if(digitalRead(BUTTON_PIN1))
				inputs[BUTTON_INDEX1].value = 255;
			else
				inputs[BUTTON_INDEX1].value = 0x00;

            if(digitalRead(BUTTON_PIN2))
				inputs[BUTTON_INDEX2].value = 255;
			else
				inputs[BUTTON_INDEX2].value = 0x00;

			snsState= PRINT_OUT;
			break;
        case PRINT_OUT:
			//Dump sampled data in the UART
			/*Serial.print(F("T:"));Serial.println(inputs[TEMP_INDEX].value,2);
			Serial.print(F("B0:"));Serial.println(inputs[BUTTON_INDEX0].value,DEC);
			Serial.print(F("B1:"));Serial.println(inputs[BUTTON_INDEX1].value,DEC);
			Serial.print(F("B2:"));Serial.println(inputs[BUTTON_INDEX2].value,DEC);*/
			snsState= GET_TEMP;                
			break;
	}
}

float SensorsTask::calcTemp(int raw)
{
    float miliVolts = (float)((float)raw*(float)ArduinoVRef/1024)*1000;
    return miliVolts/10;
}

/****************************************************************************/