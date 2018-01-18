/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */


#include "HardwareSerial.h"
#include "GroBot_Variables.h"	
#include "SampleMethods.h"
//#include "SensorsTask.h"
#include <Arduino.h>	


/****************************************************************************/


#define MAX_TEMP_SAMPLE 10


//Used for possible calib
uint32_t rawValue;
unsigned char iSample = 0;
int counterMoist = 0;




volatile uint8_t portchistory = 0xFF;     // default is high because the pull-up
uint32_t startSecondsMoist = 0;
uint8_t changedbits;

//EEPROMUtils vEEPROMUtils;

SampleMethods::SampleMethods(void)
{
	#ifdef TANKBOT_SCR
		//ads.begin();
		//ads.setGain(GAIN_ONE);
	#endif
	
}

int SampleMethods::getuSAnalogRead(unsigned char sensor)
{
	#ifdef TANKBOT_SCR
	switch (sensor)
		{
			case 1:
			  
			  //Serial.print(F("AIN0: ")); Serial.println(rawValue);
			  //rawValue = 
			iSample  = 0;
			rawValue = 0;
			for(iSample = 0; iSample< MAX_PH_SAMPLE; iSample++)
			{
				rawValue +=  phADC.calcRollingAVG();	   
			}
			rawValue = rawValue/MAX_PH_SAMPLE;
				
			//Serial.print("PHRAW: "); Serial.println(rawValue);
			  

			  
			break;
			
			case 2:
				
			iSample  = 0;
			rawValue = 0;
			for(iSample = 0; iSample< MAX_EC_SAMPLE; iSample++)
			{
				rawValue +=  ecADC.calcRollingAVG();
			}
			rawValue = rawValue/MAX_EC_SAMPLE;
			
			//Serial.print("ECRAW: "); Serial.println(rawValue);

			  
			break;
			
			case 3:
				
			iSample  = 0;
			rawValue = 0;
			for(iSample = 0; iSample< MAX_TEMP_SAMPLE; iSample++)
			{
				rawValue +=  analogRead(WATER_TEMP);
			}
			rawValue = rawValue/MAX_TEMP_SAMPLE;
				/*Serial.println("|ADCRaw|ADCRollingAVG|ADCExpAVG|ADCCOMP|");
				Serial.print("|");
				Serial.print(i2cADC.readI2CADC());
				Serial.print("|");*/
				//rawValue =  i2cADC.readI2CADC();
				/*Serial.print("|");
				Serial.print(i2cADC.calcEMAVG());
				Serial.print("|");
				Serial.print(i2cADC.readI2CADC());*/
			  //Serial.print("AIN0: "); Serial.println(rawValue);
			  /*rawValue = ads.readADC_SingleEnded(1);
			  Serial.print("AIN1: "); Serial.println(rawValue);*/

			  
			break;
		}
		return rawValue;
	#endif

    #if defined(SOILBOT_SCR)
        int newRaw = 0;
        switch (sensor)
		{
			case 1:                
                counterMoist1 = getAnalogAverageValue(MOIST_1_INDEX,MOIST_1, 3);
                newRaw = counterMoist1;
                break;
             
			case 2:               
                counterMoist2 = getAnalogAverageValue(MOIST_2_INDEX,MOIST_2, 3);
                newRaw = counterMoist2;
                break;
                
			case 5:                
                counterMoist3 = getAnalogAverageValue(MOIST_3_INDEX,MOIST_3, 3);
                newRaw = counterMoist3;
                break;
			
			case 6:                
                counterMoist4 = getAnalogAverageValue(MOIST_4_INDEX,MOIST_4, 3);
                newRaw = counterMoist4;
                break;
              		
		}
        return newRaw;            
        
    #endif
	#ifdef USERBOT_SCR
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
	#endif
	
}
#ifdef SOILBOT_SCR
int SampleMethods::getAnalogAverageValue(int index,int pin, int times2Sample)
{
	int rawValue = 0;
	int iSample = 0;
	
	//Serial.print(F(" DA:"));Serial.println(analogRead(pin),DEC);

	for(iSample = 0; iSample< times2Sample; iSample++)
	{
		rawValue +=  analogRead(pin);		
	    
	}
	rawValue = rawValue/times2Sample;
	//Serial.print(F(" Raw:"));Serial.println(rawValue,DEC);	
	return rawValue;
}	
#endif
/****************************************************************************/