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



sensorsMachineState snsState;
unsigned char stMachineExternalControl = 0x00;

#define ArduinoVRef  5

int time= 0;

SensorsTask::SensorsTask(void)
{
	snsState = INIT_SENSORS;

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
    
    //Initialize the timer to verify backoff/cooldown feature
    ticksOut1 = millis();
    ticksOut2 = millis();
    ticksOut3 = millis();
    ticksOut4 = millis();
    ticksOut5 = millis();
    ticksOut6 = millis();
    ticksOut7 = millis();
    ticksOut8 = millis();
//pinMode(outputs[OUTPUT_INDEX0].arduinoPin, OUTPUT);	

}

void SensorsTask::GoSensorsTask(void)
{
    //You can create has many states as you would like, just look at SensorsTask.h and change the enum states
    //Make sure you also implement the state on this file
    //This example has a state to initialize, other to get the temperature from an analogue sensor, third one to poll all buttons and a fourth to print the data gathered
	
	snsState = ACT_ON_IOS;
	
	
	switch(snsState)
    {
		case INIT_SENSORS:
			//Use this state in the machine to initialize any sensor you may need
			snsState = ACT_ON_IOS;
			break;
	
		case ACT_ON_IOS:		
			//Act on Output 1
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
			}
            //Act on Output 2
			if(outputs[OUTPUT_INDEX1].value>0)
			{
				if((millis() - ticksOut2) > (MILLIS_PER_MINUTE * minutes2BackOffOut2) )
				{
					if(outputs[OUTPUT_INDEX1].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX1].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX1].arduinoPin, outputs[OUTPUT_INDEX1].speed);  
				} 
			}
			else
			{
				digitalWrite(outputs[OUTPUT_INDEX1].arduinoPin, LOW);
			}
            //Act on Output 3
			if(outputs[OUTPUT_INDEX2].value>0)
			{
				if((millis() - ticksOut3) > (MILLIS_PER_MINUTE * minutes2BackOffOut3) )
				{
					if(outputs[OUTPUT_INDEX2].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX2].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX2].arduinoPin, outputs[OUTPUT_INDEX2].speed);  
				} 
			}
			else
			{
				digitalWrite(outputs[OUTPUT_INDEX2].arduinoPin, LOW);
			}
            //Act on Output 4
			if(outputs[OUTPUT_INDEX3].value>0)
			{
				if((millis() - ticksOut4) > (MILLIS_PER_MINUTE * minutes2BackOffOut4) )
				{
					if(outputs[OUTPUT_INDEX3].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX3].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX3].arduinoPin, outputs[OUTPUT_INDEX3].speed);  
				} 
			}
			else
			{
				digitalWrite(outputs[OUTPUT_INDEX3].arduinoPin, LOW);
			}
            //Act on Output 5
			if(outputs[OUTPUT_INDEX4].value>0)
			{
				if((millis() - ticksOut5) > (MILLIS_PER_MINUTE * minutes2BackOffOut5) )
				{
					if(outputs[OUTPUT_INDEX4].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX4].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX4].arduinoPin, outputs[OUTPUT_INDEX4].speed);  
				} 
			}
			else
			{
				digitalWrite(outputs[OUTPUT_INDEX4].arduinoPin, LOW);
			}
            //Act on Output 6
			if(outputs[OUTPUT_INDEX5].value>0)
			{
				if((millis() - ticksOut6) > (MILLIS_PER_MINUTE * minutes2BackOffOut6) )
				{
					if(outputs[OUTPUT_INDEX5].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX5].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX5].arduinoPin, outputs[OUTPUT_INDEX5].speed);  
				} 
			}
			else
			{
				digitalWrite(outputs[OUTPUT_INDEX5].arduinoPin, LOW);
			}
            //Act on Output 7
			if(outputs[OUTPUT_INDEX6].value>0)
			{
				if((millis() - ticksOut7) > (MILLIS_PER_MINUTE * minutes2BackOffOut7) )
				{
					if(outputs[OUTPUT_INDEX6].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX6].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX6].arduinoPin, outputs[OUTPUT_INDEX6].speed);  
				} 
			}
			else
			{

				digitalWrite(outputs[OUTPUT_INDEX6].arduinoPin, LOW);
			}
            //Act on Output 8
			if(outputs[OUTPUT_INDEX7].value>0)
			{
				if((millis() - ticksOut8) > (MILLIS_PER_MINUTE * minutes2BackOffOut8) )
				{
					if(outputs[OUTPUT_INDEX7].speed == 0)
						digitalWrite(outputs[OUTPUT_INDEX7].arduinoPin, HIGH);
					else
						analogWrite(outputs[OUTPUT_INDEX7].arduinoPin, outputs[OUTPUT_INDEX7].speed);  
				} 
			}
			else
			{
				digitalWrite(outputs[OUTPUT_INDEX7].arduinoPin, LOW);
			}
            
			snsState = INIT_SENSORS;
			break;      
	}
}

void SensorsTask::printOutputState(unsigned char indexOutput)
{
    Serial.print(F("Output "));
    Serial.print(indexOutput+1,DEC);
    Serial.print(F("- "));
    
    if(outputs[indexOutput].value == 0)
    {
        Serial.print(F("OFF Speed-"));
    }
    else
    {
        Serial.print(F("ON Speed-"));
    }		
    Serial.println(outputs[indexOutput].speed,DEC);
    Serial.println("");

}



/****************************************************************************/