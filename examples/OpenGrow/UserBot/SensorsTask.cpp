/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "GroBot_Variables.h"
#include "SensorsTask.h"
#include <Arduino.h>
#include "ComsTask.h"

#define MAX_TEMP_SAMPLE 10

sensorsMachineState snsState;

#define ArduinoVRef  5

int time= 0;

SensorsTask::SensorsTask(void)
{
	snsState = INIT_SENSORS;

	//***********************  IO Config  *****************************//
    /*Here you need to set your used pins as inputs or outputs*/
	pinMode(inputs[INPUT_INDEX0].arduinoPin, INPUT);
    pinMode(inputs[INPUT_INDEX1].arduinoPin, INPUT);

	pinMode(outputs[OUTPUT_INDEX0].arduinoPin, OUTPUT);		
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
			time = getAnalogRead(inputs[INPUT_INDEX1].arduinoPin);
			inputs[INPUT_INDEX1].value += calcTemp(time);
			inputs[INPUT_INDEX1].value = inputs[INPUT_INDEX1].value/2;                        
			snsState = GET_IOS;
			break;		
			
		case GET_IOS:
            //Polling button
			if(digitalRead(inputs[INPUT_INDEX0].arduinoPin))
				inputs[INPUT_INDEX0].value = 255;
			else
				inputs[INPUT_INDEX0].value = 0x00;          

			snsState= ACT_ON_IOS;
			break;
			
	
		case ACT_ON_IOS:
		
		//Act on LED pin
		if(outputs[OUTPUT_INDEX0].value>0)
		{
			if((millis() - ticksOut1) > (MILLIS_PER_MINUTE * minutes2BackOffOut1) )
			{
				if(outputs[OUTPUT_INDEX0].speed == 0)
					digitalWrite(outputs[OUTPUT_INDEX0].arduinoPin, HIGH);
				else
					analogWrite(outputs[OUTPUT_INDEX0].arduinoPin, outputs[OUTPUT_INDEX0].speed);  
			} 
		}
		else
		{
			digitalWrite(outputs[OUTPUT_INDEX0].arduinoPin, LOW);
		   //Serial.println("11");

		}
		snsState = GET_TEMP;

		break;
      
	}
}

float SensorsTask::calcTemp(int raw)
{
    float miliVolts = (float)((float)raw*(float)ArduinoVRef/1024)*1000;
    return miliVolts/10;
}

float SensorsTask::getAnalogRead(unsigned char pin)
{
	uint32_t rawValue= 0;
	unsigned char iSample = 0;
                       
	iSample  = 0;
	rawValue = 0;
	
	for(iSample = 0; iSample< MAX_TEMP_SAMPLE; iSample++)
	{
		rawValue +=  analogRead(pin);
	}
	
	rawValue = rawValue/MAX_TEMP_SAMPLE;
	return rawValue;            
			
	
}

/****************************************************************************/