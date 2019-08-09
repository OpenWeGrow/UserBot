/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "HardwareSerial.h"
#include "GroBot_Variables.h"		
#include "SerialTask.h"
#include "ComsTask.h"
#include "EEPROM_Utils.h"
#include "RF24.h"
/****************************************************************************/

//State Machine Stuff
serialMachineState stState;

#define DISC_INSERT_DATA "Insert:"
#define DISC_SHOW_DATA "New:"
#define DISC_SAVED_DATA "Saved!"

unsigned char sti=0;
unsigned char read = 0;
char uartData [15];
String receiveString;
int getSingleChar;
char c;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

SerialTask::SerialTask(void)
{
	stState = CHECK_SERIAL;	
}

void SerialTask::vGoSerialTask(RF24 comPort)
{          
	switch(stState)
	{	
		case CHECK_SERIAL:
            
            if (Serial.available() > 0) 
            {
                
              c = Serial.read();
         
			 
			  if ( c == 'D')
			  {
				printDisclamer();
                comPort.printDetails();
			  }
			  else if ( c == 'R') //Full Reset of EEPROM to default
			  {
				vEEPROMUtils.resetGroBot2Defaults();
			  } 
			  else if(c == 'P')
			  {	
				vEEPROMUtils.dumpEEPROM();
			  }
			  else if(c == 'S')
			  {
				 read = 0;
				 sti = 0;
				 stState = GET_DATA;	
				 Serial.println(c);				 
			     Serial.println(DISC_INSERT_DATA);				 
			  }    
			  else if(c == 'M')
			  {
				 stState = GET_DATA;	
				 Serial.println(c);				 
			     Serial.println(DISC_INSERT_DATA);				 
			  }             
			  if(stState!=GET_DATA)
              {                  
                  c=' ';
              }
            
            }
		break;
		
		case GET_DATA:
			
			
			
			switch(c)
			{
				case ('S'):
                    
                if (Serial.available() > 0) 
                {
      
                    c = Serial.read();
                   
                    while(sti<10)
                    {
                        if (Serial.available() > 0) 
                        {
                            uartData[sti++] = c;	
                            c = Serial.read();
                        }						
                    }
                    
                    unsigned long longSerialNumber;
					
					if(sti == 10)
					{
						sti= 0;
						read= 0;
						stState = CHECK_SERIAL;
						
						//Lock String and Remove first type digit
						uartData[0]= 0x30; //0x00 in ASCII
						uartData[10]= '\0';
						longSerialNumber = 0;
						
						//Convert String to Long
						longSerialNumber = strtoul( uartData, NULL, 10 );
						sprintf(uartData, "%lu", longSerialNumber);

						//Convert Long to unsigned char Array
						serialNumber[7] =  (unsigned char)(longSerialNumber & 0xFF);
						serialNumber[6] =  (unsigned char)(longSerialNumber >> 8 & 0xFF);
						serialNumber[5] =  (unsigned char)(longSerialNumber >> 16 & 0xFF);
						serialNumber[4] =  (unsigned char)(longSerialNumber >> 24 & 0xFF);

						vEEPROMUtils.saveModuleConfig(SAVE_SERIALNUMBER);
						Serial.println(DISC_SAVED_DATA);
				
						resetFunc();
					
					}
                }
				
				break;
    
                
				case 'M':
                    
                    if (Serial.available() > 0) 
                    {
            
                        c = Serial.read();
                        uartData[0] = c;
                        uartData[1] = '\0';
                        masterID = atoi(uartData);
                        vEEPROMUtils.saveModuleConfig(SAVE_MASTERID);
						vEEPROMUtils.saveModuleConfig(SAVE_INITSTATE);
						Serial.println(DISC_SAVED_DATA);
		
						resetFunc();
                    }
				
				break;
			}
			
		break;
	}
  
}


void SerialTask::printDisclamer(void)
{
    //Serial.flush();
    unsigned long longSerialNumber;
    unsigned char sti = 0;
	char uartData [15];
	longSerialNumber = 0;
	longSerialNumber = (longSerialNumber | serialNumber[4]) << 8;
	longSerialNumber = (longSerialNumber | serialNumber[5]) << 8;
	longSerialNumber = (longSerialNumber | serialNumber[6]) << 8;
	longSerialNumber = (longSerialNumber | serialNumber[7]);

	Serial.println("UserBot! Ready");


	Serial.print(F("N: \t"));
	Serial.println(name);	
	
	//Print Serial Number
	Serial.print("S: \t");
	Serial.print(botType,DEC);			
	if(longSerialNumber>0 && longSerialNumber<999999999)
	{
		sprintf(uartData, "%lu", longSerialNumber);
		if(strlen(uartData)<9);
			for (sti =0; sti < (9-strlen(uartData));sti++)
			{
				Serial.print("0");
			}
		Serial.println(uartData);	
	}
	else
	{
		Serial.println(F("999999999D"));
		serialNumber[4]  = DEFAULT_SERIALNUMBER_4;
		serialNumber[5]  = DEFAULT_SERIALNUMBER_5;
		serialNumber[6]  = DEFAULT_SERIALNUMBER_6;
		serialNumber[7]  = DEFAULT_SERIALNUMBER_7;
		vEEPROMUtils.saveModuleConfig(SAVE_SERIALNUMBER);
	}
	//Print Master ID
	Serial.print("MID: \t");
	Serial.println(masterID,DEC);	
	
	//Print Slave Adress
	Serial.print("SAdd: \t");
	Serial.println(slaveAddress,HEX);	

    Serial.print("FW: \t");
	Serial.print(fwVersion[0],DEC);
	Serial.print(".");
    Serial.print(fwVersion[1],DEC);
    Serial.print(".");
    Serial.print(fwVersion[2],DEC);
	Serial.print(".");
    Serial.println(fwVersion[3],DEC);

        

}


/****************************************************************************/