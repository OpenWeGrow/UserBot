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

#define MAX_USER_OUTPUTS 8

sensorsMachineState snsState;
unsigned char stMachineExternalControl = 0x00;

#define ArduinoVRef  5

int time= 0;
unsigned char outputIndex2Act = 0;

SensorsTask::SensorsTask(void)
{
	snsState = INIT_SENSORS;
}

void SensorsTask::GoSensorsTask(void)
{
    //You can create has many states as you would like, just look at SensorsTask.h and change the enum states
    //Make sure you also implement the state on this file
    //This example has a state to initialize, and other to constantly check if any output changed
	
	//snsState = ACT_ON_IOS;
	
	
	switch(snsState)
    {
		case INIT_SENSORS:
            //Use this state in the machine to initialize any sensor you may need
                    
            

            //Initialize the timer to verify backoff/cooldown feature
            ticksOut1 = millis();
            ticksOut2 = millis();
            ticksOut3 = millis();
            ticksOut4 = millis();
            ticksOut5 = millis();
            ticksOut6 = millis();
            ticksOut7 = millis();
            ticksOut8 = millis();
            
			snsState = ACT_ON_IOS;
			break;
	
		case ACT_ON_IOS:		
            //Act on Output 1
            for(outputIndex2Act =0 ; outputIndex2Act<MAX_USER_OUTPUTS; outputIndex2Act++)
                actOnOutput(outputIndex2Act); 
            
			snsState = ACT_ON_IOS;
			break;      
	}
}

void SensorsTask::actOnOutput(unsigned char indexOutput)
{
    unsigned long outputTicks;
    unsigned char outputMinutes2BackOff;
    
    //Search the correct timeout for the cooldown feature
    switch(indexOutput)
    {
        case OUTPUT_INDEX0:
            outputTicks = ticksOut1;
            outputMinutes2BackOff = minutes2BackOffOut1;
        break;
        
        case OUTPUT_INDEX1:
            outputTicks = ticksOut2;
            outputMinutes2BackOff = minutes2BackOffOut2;
        break;
        
        case OUTPUT_INDEX2:
            outputTicks = ticksOut3;
            outputMinutes2BackOff = minutes2BackOffOut3;
        break;
        
        case OUTPUT_INDEX3:
            outputTicks = ticksOut4;
            outputMinutes2BackOff = minutes2BackOffOut4;
        break;
        
        case OUTPUT_INDEX4:
            outputTicks = ticksOut5;
            outputMinutes2BackOff = minutes2BackOffOut5;
        break;
        
        case OUTPUT_INDEX5:
            outputTicks = ticksOut6;
            outputMinutes2BackOff = minutes2BackOffOut6;
        break;
        
        case OUTPUT_INDEX6:
            outputTicks = ticksOut7;
            outputMinutes2BackOff = minutes2BackOffOut7;
        break;
        
        case OUTPUT_INDEX7:
            outputTicks = ticksOut8;
            outputMinutes2BackOff = minutes2BackOffOut8;
        break;
        
        case OUTPUT_INDEX8:
            outputTicks = ticksOut9;
            outputMinutes2BackOff = minutes2BackOffOut9;
        break;
        
        case OUTPUT_INDEX9:
            outputTicks = ticksOut10;
            outputMinutes2BackOff = minutes2BackOffOut10;
        break;
            
    }
    

    if(outputs[indexOutput].value>0)
    {
        //If not in cooldown turn on the output
        if((millis() - outputTicks) > (MILLIS_PER_MINUTE * outputMinutes2BackOff) )
        {
            if(outputs[indexOutput].speed == 0)
                digitalWrite(outputs[indexOutput].arduinoPin, HIGH);
            else
                analogWrite(outputs[indexOutput].arduinoPin, outputs[indexOutput].speed);  
        } 
    }
    else
    {
        //Shutdown the output, no cooldown verification needed
        digitalWrite(outputs[indexOutput].arduinoPin, LOW);
    }   
    
}


void SensorsTask::printOutputState(unsigned char indexOutput)
{
    
    Serial.print(F("Output "));
    Serial.print(indexOutput+1,DEC);
    Serial.print(F("- "));
    
        if(reversedOutputs)
        {
            if(outputs[indexOutput].value == 0)
            {
                Serial.print(F("ON Speed-"));
            }
            else
            {
                Serial.print(F("OFF Speed-"));
            }
        }
        else
        {
            if(outputs[indexOutput].value == 0)
            {
                Serial.print(F("OFF Speed-"));
            }
            else
            {
                Serial.print(F("ON Speed-"));
            }		
        }
    
    Serial.print(outputs[indexOutput].speed,DEC);
    Serial.print(" CoolDown: ");
    
    switch(indexOutput)
    {
        case OUTPUT_INDEX0:
            Serial.print(minutes2BackOffOut1);
            Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX1:
            Serial.print(minutes2BackOffOut2);
            Serial.println(" minutes");
            break;            
            
        case OUTPUT_INDEX2:
            Serial.print(minutes2BackOffOut3);
            Serial.println(" minutes");
            break;
            
            
        case OUTPUT_INDEX3:
            Serial.print(minutes2BackOffOut4);
            Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX4:
            Serial.print(minutes2BackOffOut5);
            Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX5:
            Serial.print(minutes2BackOffOut6);
            Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX6:
            Serial.print(minutes2BackOffOut7);
            Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX7:
            Serial.print(minutes2BackOffOut8);
            Serial.println(" minutes");
            break;
    }
    
  

}



/****************************************************************************/