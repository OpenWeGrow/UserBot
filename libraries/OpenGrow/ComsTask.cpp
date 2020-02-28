/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "GroBot_Variables.h"
#include "CRC16.h"
#include "ComsTask.h"
#include "OpenBus.h"
#include <EEPROM.h>
#include "Arduino.h"

#define GRONODE_TIMEOUT 30000

unsigned char need2Shutdown = false;
unsigned long timeout2Shutdown = 0;
unsigned char shutdownFlag = 1; //Default shutdown ON
unsigned long shutdownTimeout = 60000; //Default waiting time 1 minute
unsigned long ticksOut1 = 0;
unsigned long ticksOut2 = 0;
unsigned long ticksOut3 = 0;
unsigned long ticksOut4 = 0;
unsigned long ticksOut5 = 0;
unsigned long ticksOut6 = 0;
unsigned long ticksOut7 = 0;
unsigned long ticksOut8 = 0;
unsigned long ticksOut9 = 0;
unsigned long ticksOut10 = 0;

/****************************************************************************/
EEPROMUtils vEEPROMUtils;

//Module Definitions
unsigned char slaveAddress;
unsigned char serialNumber[8];
char name[15]="New Module";
unsigned char masterID;
unsigned char botType;
unsigned char botID;
unsigned char reboot = 0x00;

//Sensors Definitions
struct stINPUT inputs[MAX_INPUTS];
struct stOUTPUT outputs[MAX_OUTPUTS];
unsigned char fwVersion [4];

machineState mState;
bool comOk = false;	

//Timeout Stuff
uint32_t long started_waiting_at = 0;
bool timeout = false;

//Cycles Stuff
unsigned short i = 0;

//Memory addressing
unsigned short memadd = 0;
#if !defined(RS485_INTERFACE) && defined(DEBUGCOMSTASK)
unsigned short d = 0;
#endif

bool reversedOutputs = false; //Used to reverse polarity of Output On or Output Off

OpenBus vOpenBus;

//Memory init stuff
unsigned char firstConnect=0x00;

//Random timing for authentication
//Used to avoid collision between authenticating modules
long randNumber;

ComsTask::ComsTask(void)
{
	mState = INIT;
}

void ComsTask::vSetReversedOutputs(bool newReversedOutputs)
{
    reversedOutputs = newReversedOutputs;
}

void ComsTask::vGoComsTask(RF24 comPort)
{
    switch(mState)
    {
		case INIT:
			firstConnect = EEPROM.read(EEPROM_SLAVE_INIT_STATE); //See if we were awake before
			fwVersion[0] =   EEPROM.read(FW_EEPROM_START);
			fwVersion[1] =   EEPROM.read(FW_EEPROM_START+1);
			fwVersion[2] =   EEPROM.read(FW_EEPROM_START+2);
			fwVersion[3] =   EEPROM.read(FW_EEPROM_START+3);

			timeout2Shutdown = millis();

			//Read the force shutdown flags and timeouts
			shutdownFlag = EEPROM.read(EEPROM_SHUTDWON_FLAG);
			eeprom_read_block(&shutdownTimeout, (void *)(EEPROM_SHUTDWON_TIMEOUT), sizeof(shutdownTimeout));

			if(shutdownFlag == 0xFF)
			{
				shutdownFlag= 1;
				shutdownTimeout = 60000;
				vEEPROMUtils.vSaveShutdown();
			}

			if(firstConnect == MODULE_CONFIGED) //Check if we had a previous communication and got a Slave Address
			{
				slaveAddress = 0xFF; //Get always a new slave address

				masterID = EEPROM.read(EEPROM_MASTER_ID);	//If we already have a grow definition read it form memory
				botID = EEPROM.read(EEPROM_BOT_ID);			//If we already have a grow definition read it form memory
				
				//Protection for Wrong Master ID
				if(masterID == 0 || masterID > 5)
				{
					masterID = 5;
					#if !defined(RS485_INTERFACE) && (defined(DEBUGCOMSTASK) || defined(GENERALDEBUG))
						Serial.print("MasterID Reseted");	
					#endif
				}
			}
			else
			{
				memset(name,0x00,sizeof(name)); //Clear the buffer

				strcpy(name,"New UserBot");
				randomSeed(analogRead(A7)); //Needs validation cause A7 can be connected!!!!!!

				slaveAddress = OPEN_SLAVEADRESS_DEFAULT; //If not reset SlaveAdress to get new one
				masterID= OPEN_MASTERID_DEFAULT;
				botID = OPEN_BOT_ID_DEFAULT;
			}

			//No matter what always read the content of our serialNumber
			eeprom_read_block(serialNumber, (void *)(EEPROM_SERIALNUMBER), sizeof(serialNumber));

			botType = EEPROM.read(EEPROM_BOT_TYPE);

			mState=SEARCH_MASTER;

			//Configure the common address bytes for the different pipes adding masterID*3 to the pipes default
			pipes[0] += ((uint64_t)(masterID*3)<<32);
			pipes[0] += ((uint64_t)(masterID*3)<<24);
			pipes[0] += ((uint64_t)(masterID*3)<<16);
			pipes[0] += ((uint64_t)(masterID*3)<<8);

			pipes[1] += ((uint64_t)(masterID*3)<<32);
			pipes[1] += ((uint64_t)(masterID*3)<<24);
			pipes[1] += ((uint64_t)(masterID*3)<<16);
			pipes[1] += ((uint64_t)(masterID*3)<<8);

			pipes[2] += ((uint64_t)(masterID*3)<<32);
			pipes[2] += ((uint64_t)(masterID*3)<<24);
			pipes[2] += ((uint64_t)(masterID*3)<<16);
			pipes[2] += ((uint64_t)(masterID*3)<<8);

			pipes[3] += ((uint64_t)(masterID*3)<<32);
			pipes[3] += ((uint64_t)(masterID*3)<<24);
			pipes[3] += ((uint64_t)(masterID*3)<<16);
			pipes[3] += ((uint64_t)(masterID*3)<<8);

			pipes[4] += ((uint64_t)(masterID*3)<<32);
			pipes[4] += ((uint64_t)(masterID*3)<<24);
			pipes[4] += ((uint64_t)(masterID*3)<<16);
			pipes[4] += ((uint64_t)(masterID*3)<<8);

			pipes[5] += ((uint64_t)(masterID*3)<<32);
			pipes[5] += ((uint64_t)(masterID*3)<<24);
			pipes[5] += ((uint64_t)(masterID*3)<<16);
			pipes[5] += ((uint64_t)(masterID*3)<<8);

			comPort.begin();
			comPort.setRetries(15,15);
			comPort.openReadingPipe(0,pipes[AUTH_CHANNEL]);

			started_waiting_at = millis();
			break;

		case SEARCH_MASTER:
			randNumber = random(3, 10);
			slaveAddress = 0xFF; //Get always a new slave address

			if((millis() - started_waiting_at < (randNumber*1000)))
			{            
				break;
			}
			else
			{
				memset(data,0x00,256); //Clear the buffer

				comPort.openWritingPipe(pipes[AUTH_CHANNEL]);
				comPort.openReadingPipe(0,pipes[AUTH_CHANNEL]);
				Serial.println("A!");
				if(vOpenBus.usOpenBusCreateAuthFrame(data, comPort, AUTH_CHANNEL)) //Build and send Authentication Frame
				{   
					//Getting Reply and send acknowledge frame
					if(vOpenBus.usOpenBusReply(data,data,comPort,AUTH_CHANNEL))
					{  
						Serial.println("MF");
						comPort.printDetails();

						need2Shutdown = false;
						timeout2Shutdown = millis();

						mState = GET_COMMAND;
					}
				}
			}
			started_waiting_at = millis();
			break;

      case GET_COMMAND:
		//For Command Get we change listening channel from AUTH_CHANNEL to MASTER_READ_CHANNEL
		comPort.openWritingPipe(pipes[slaveAddress & 0x0F]);
		comPort.openReadingPipe(0,pipes[slaveAddress & 0x0F]);

		if((millis() - started_waiting_at < GRONODE_TIMEOUT))
			timeout = false;
		else
			timeout = true;

		if(!timeout)
		{
			memset(data,0x00,256); //Clear Buffer
			if (vOpenBus.usOpenBusReply(data,data,comPort,slaveAddress & 0x0F))
			{	             
				started_waiting_at = millis();
				timeout2Shutdown = millis();
			}
			if(reboot)
			{
			  masterID=reboot;
			  vEEPROMUtils.saveModuleConfig(SAVE_MASTERID);
			  vEEPROMUtils.saveModuleConfig(SAVE_INITSTATE);
			  resetFuncCom();
			}
		}
		else
		{
			timeout = false;
			started_waiting_at = millis();
			mState = SEARCH_MASTER;
			Serial.println("ML");
			need2Shutdown = true;
			timeout2Shutdown = millis();
		}
		break;  
    }
}

/****************************************************************************/