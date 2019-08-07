/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "GroBot_Variables.h"	
#include "SampleMethods.h"
#include <Arduino.h>	

/****************************************************************************/
//Number of samples to take of the analog input
#define MAX_TEMP_SAMPLE 10

uint32_t rawValue= 0;
unsigned char iSample = 0;

SampleMethods::SampleMethods(void)
{
	
}

int SampleMethods::getuSAnalogRead(unsigned char sensor)
{

		switch (sensor)
		{
			case 1:
                       
				iSample  = 0;
				rawValue = 0;
				for(iSample = 0; iSample< MAX_TEMP_SAMPLE; iSample++)
				{
					rawValue +=  analogRead(TEMP_PIN);
				}
				rawValue = rawValue/MAX_TEMP_SAMPLE;
				return rawValue;            
			
		
		}

}

/****************************************************************************/