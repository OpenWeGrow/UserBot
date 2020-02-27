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

SensorsTask::SensorsTask(void)
{
	snsState = INIT_OUTPUTS;

	//***********************  IO Config  *****************************//
    /*Here you need to set your used pins as inputs or outputs*/
	
}

void SensorsTask::GoSensorsTask(void)
{
    //You can create has many states as you would like, just look at SensorsTask.h and change the enum states
    //Make sure you also implement the state on this file
    //This example has two states to initialize outputs and inputs, and other to act on the output
	if(stMachineExternalControl)
	{
		stMachineExternalControl = 0x00;
		snsState = ACT_ON_IOS;
	}
	
	switch(snsState)
    {
        case INIT_OUTPUTS:
			//Use this state in the machine to initialize any sensor you may need
            pinMode(outputs[OUTPUT_INDEX0].arduinoPin, OUTPUT);		
			snsState = INIT_SENSORS;
			break;
            
		case INIT_SENSORS:
			//Use this state in the machine to initialize any sensor you may need
			snsState = ACT_ON_IOS;
			break;
	
		
		case ACT_ON_IOS:	
			
			//Act on DC Motor			
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
			snsState = INIT_SENSORS;
			break;      
	}
}

/****************************************************************************/