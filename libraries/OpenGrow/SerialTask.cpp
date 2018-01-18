/*
*	Copyright (C) Open Grow 2014 
*	Coders: João Melo
*/

#include "HardwareSerial.h"
#include "GroBot_Variables.h"		
#include "SerialTask.h"
#include "ComsTask.h"
#include "EEPROM_Utils.h"
#include "SampleMethods.h"
#include "RF24.h"
/****************************************************************************/

//State Machine Stuff
serialMachineState stState;

//Sensors Definitions
//struct stINPUT inputs[MAX_INPUTS];
//struct stOUTPUT outputs[MAX_OUTPUTS];
#define DISC_INSERT_DATA "Insert:"
#define DISC_SHOW_DATA "New:"
#define DISC_SAVED_DATA "Saved!"

//String receiveString;



unsigned char sti=0;
unsigned char read = 0;
char uartData [15];
String receiveString;
int getSingleChar;

char c;

void(* resetFunc) (void) = 0; //declare reset function @ address 0


#ifdef SOILBOT_SCR
//unsigned long sensorCalibValue = 0;
#endif


//EEPROMUtils vEEPROMUtils;

SerialTask::SerialTask(void)
{
	stState = CHECK_SERIAL;	
}

void SerialTask::vGoSerialTask(RF24 comPort)

{
    
    //unsigned char read = 0;
    //unsigned char sti=0;
    //char uartData [15];
    
	switch(stState)
	{	
		case CHECK_SERIAL:
            
            if (Serial.available() > 0) 
            {
                //Serial.println("Serial Available");
                c = Serial.read();
            //Serial.print("Checking Serial: ");
			/*if ( receiveString.length() == 1)
			{*/
			 // c = Serial.read();
			 
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
                
              else if(c == 'A')
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
              else if(c == 'I')
			  {
				 printBotData();			 
			  }
			 		  
	
			  
			  if(stState!=GET_DATA)
              {
                  
                  c=' ';
              }
                
              
			//}
            }
		break;
		
		case GET_DATA:
			
			
			
			switch(c)
			{
				case ('S'):
                    
                if (Serial.available() > 0) 
                {
                    //Serial.println("Serial Available");
                    c = Serial.read();
                   
                    while(sti<10)
                    {
                        /*Serial.println(receiveString.length() ,DEC);
                        Serial.println(read ,DEC);
                        Serial.println(sti,DEC);*/
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
                
                #if defined (RS485_INTERFACE)
                    case 'A':

                        if (Serial.available() > 0) 
                        {
                   
                            c = Serial.read();
                            uartData[0] = c;                        
                            uartData[1] = '\0';

                            slaveAddress = BASE_RS485_ADRESS



                            slaveAddress += atoi(uartData);
                            vEEPROMUtils.saveModuleConfig(SAVE_SLAVEADRESS);
            
                            resetFunc();
                        }			

                    break;
                #endif
                
				case 'M':
                    
                    if (Serial.available() > 0) 
                    {
                        //Serial.println("Serial Available");
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
	//Serial.print("\n\r");
	
	//Print Slave Adress
	Serial.print("SAdd: \t");
	Serial.println(slaveAddress,HEX);	
	//Serial.println("\n\r");

    Serial.print("FW: \t");
	Serial.print(fwVersion[0],DEC);
	Serial.print(".");
    Serial.print(fwVersion[1],DEC);
    Serial.print(".");
    Serial.print(fwVersion[2],DEC);
	Serial.print(".");
    Serial.println(fwVersion[3],DEC);
	//Serial.println();
        

}

void SerialTask::printBotData(void)
{

        /*Serial.println("**** Data Start ****");
        Serial.print("Temp:");Serial.println(inputs[WATER_TEMP_INDEX].value,DEC);
        Serial.print("PH:");Serial.print(inputs[PH_INDEX].value,DEC);Serial.print(" Raw:");Serial.println(vSampleMethods.getuSAnalogRead(1),DEC);
        vEEPROMUtils.vReadCalib(PH_INDEX+1);
        Serial.print("Max: ");
        Serial.print(CalibParams.timeMax);
        Serial.print("  Min: ");
        Serial.print(CalibParams.timeMin);
        Serial.print("  M: ");
        Serial.print(CalibParams.m);
        Serial.println(" %");
        Serial.print("EC:");Serial.print(inputs[EC_INDEX].value,DEC);Serial.print(" Raw:");Serial.println(vSampleMethods.getuSAnalogRead(2),DEC);
        vEEPROMUtils.vReadCalib(EC_INDEX+1);
        Serial.print("Max: ");
        Serial.print(CalibParams.timeMax);
        Serial.print("  Min: ");
        Serial.print(CalibParams.timeMin);
        Serial.print("  M: ");
        Serial.print(CalibParams.m);
        Serial.println(" %");
        Serial.print("I1:");Serial.println(inputs[INPUTS_LVL_1_INDEX].value,DEC);
        Serial.print("I2:");Serial.println(inputs[INPUTS_LVL_2_INDEX].value,DEC);
        Serial.print("I3:");Serial.println(inputs[INPUTS_LVL_3_INDEX].value,DEC);
        Serial.print("I4:");Serial.println(inputs[INPUTS_LVL_4_INDEX].value,DEC);
        Serial.print("O1:");Serial.println(outputs[OUTPUTS_EV_1_INDEX].value,DEC);
        Serial.print("O2:");Serial.println(outputs[OUTPUTS_EV_2_INDEX].value,DEC);
        Serial.print("O3:");Serial.println(outputs[OUTPUTS_EV_3_INDEX].value,DEC);
        Serial.print("O4:");Serial.println(outputs[OUTPUTS_EV_4_INDEX].value,DEC);
        Serial.println("**** Data End ****");   */ 

}

/****************************************************************************/