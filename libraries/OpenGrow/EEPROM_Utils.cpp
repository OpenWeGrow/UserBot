/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "GroBot_Variables.h"
#include <EEPROM.h>
#include "HardwareSerial.h"
#include "EEPROM_Utils.h"
#include "ComsTask.h"




parameters_Moisture CalibParams = {700,400,0.33};

unsigned char minutes2BackOffOut1 = 0;
unsigned char minutes2BackOffOut2 = 0;
unsigned char minutes2BackOffOut3 = 0;
unsigned char minutes2BackOffOut4 = 0;
unsigned char minutes2BackOffOut5 = 0;
unsigned char minutes2BackOffOut6 = 0;
unsigned char minutes2BackOffOut7 = 0;
unsigned char minutes2BackOffOut8 = 0;
unsigned char minutes2BackOffOut9 = 0;
unsigned char minutes2BackOffOut10 = 0;

extern unsigned short memadd;
unsigned short ei = 0;
EEPROMUtils::EEPROMUtils()
{

}


void EEPROMUtils::resetGroBot2Defaults(void)
{
	//Reset EEPROM
		
		
		EEPROM.write(EEPROM_SERIALNUMBER, DEFAULT_SERIALNUMBER_0);   //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+1, DEFAULT_SERIALNUMBER_1); //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+2, DEFAULT_SERIALNUMBER_2); //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+3, DEFAULT_SERIALNUMBER_3); //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+4, DEFAULT_SERIALNUMBER_4); //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+5, DEFAULT_SERIALNUMBER_5); //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+6, DEFAULT_SERIALNUMBER_6); //Save Serial Number
		EEPROM.write(EEPROM_SERIALNUMBER+7, DEFAULT_SERIALNUMBER_7); //Save Serial Number
		EEPROM.write(EEPROM_SLAVE_INIT_STATE, OPEN_DEFAULT); //Module in first connection 
		EEPROM.write(EEPROM_MASTER_ID, OPEN_MASTERID_DEFAULT); //No Master ID yet
		EEPROM.write(EEPROM_BOT_ID, OPEN_BOT_ID_DEFAULT); //No Master ID yet
	
				
		//You should define your sensor types and names here
		EEPROM.write(EEPROM_BOT_TYPE, USERBOT); //Save Module Type

		strcpy(name,"User Bot");

		//Example of configuration
	   
		
		 //Define names and types
		 
		inputs[0].type = BUTTON;
		strcpy(inputs[0].name,"Button1");
		inputs[1].type = BUTTON;
		strcpy(inputs[1].name,"Button2");
		inputs[2].type = BUTTON;
		strcpy(inputs[2].name,"Button3");
		inputs[3].type = DIG_TEMPERATURE;
		strcpy(inputs[3].name,"Temperature");

		//Clear unused IO's
		for(ei=4;ei<10;ei++)
		{
				inputs[ei].type = OPEN_DEFAULT;
				strcpy(inputs[ei].name,"null");
		}

		for(ei=0;ei<10;ei++)
		{
				outputs[ei].type = OPEN_DEFAULT;
				strcpy(outputs[ei].name,"null");
		}
		
		
		//If you need calibration parameters you can use the structure below to save those parameters on the EEPROM
		CalibParams.timeMax = 700;
		CalibParams.timeMin = 400;
		CalibParams.m = 0.33;

		vSaveCalib(1);
		vSaveCalib(2);
		vSaveCalib(3);
		vSaveCalib(4);
		vSaveCalib(5);
		vSaveCalib(6);

		
		//Set your FirmwareVersion		
		fwVersion[0] = FW_1;
        fwVersion[1] = FW_2;
        fwVersion[2] = FW_3;
        fwVersion[3] = FW_4;

		eeprom_write_block(name, (void *)(EEPROM_BOT_NAME), sizeof(name)); 
        eeprom_write_block(fwVersion, (void *)(FW_EEPROM_START), sizeof(fwVersion));
		Serial.println(F("RST"));
}

void EEPROMUtils::dumpEEPROM ()
{
	for(ei = 0; ei< 0x255;ei++)
	{
		Serial.print(ei,HEX);
		Serial.println(EEPROM.read(ei),HEX);			
	}
}


void EEPROMUtils::vSaveSlaveAdress(unsigned char newSlaveAdress)
{
	EEPROM.write(EEPRROM_SLAVEADDRESS , newSlaveAdress);	
	EEPROM.write(EEPROM_SLAVE_INIT_STATE, MODULE_CONFIGED);
}

void EEPROMUtils::vSaveBotID(unsigned char newBotID)
{
	EEPROM.write(EEPROM_BOT_ID , newBotID);
}


void EEPROMUtils::vSaveShutdown()
{
	EEPROM.write(EEPROM_SHUTDWON_FLAG , shutdownFlag);	//Set New Shutdown flag
	eeprom_write_block(&shutdownTimeout, (void *)EEPROM_SHUTDWON_TIMEOUT, sizeof(shutdownTimeout));
}


void EEPROMUtils::saveModuleConfig(unsigned char saveWhat)
{
		
	switch(saveWhat)
	{
		
		case SAVE_SLAVEADRESS:
			EEPROM.write(EEPRROM_SLAVEADDRESS, slaveAddress); 
		break;		
	
		
		case SAVE_SERIALNUMBER:
			for(ei=0;ei<8;ei++) 
			{
				memadd = EEPROM_SERIALNUMBER + ei;
				EEPROM.write(memadd,   serialNumber[ei]);
			}	
		break;
		
		case SAVE_MASTERID:
			EEPROM.write(EEPROM_MASTER_ID, masterID); 
		break;
		
		case SAVE_INITSTATE:
			EEPROM.write(EEPROM_SLAVE_INIT_STATE, MODULE_CONFIGED);
		break;	
    
        case SHUTDOWN_CONFIG:
            vSaveShutdown();
            break;

		case CALIB_INPUT_1:			
			vSaveCalib(1);
		break;		

		case CALIB_INPUT_2:
			vSaveCalib(2);
		break;	

		case CALIB_INPUT_3:			
			vSaveCalib(3);
		break;	

		case CALIB_INPUT_4:
			vSaveCalib(4);
		break;	

		case CALIB_INPUT_5:
			vSaveCalib(5);
		break;	

		case CALIB_INPUT_6:
			vSaveCalib(6);
		break;	
		   
		case OUT1_BACKOFF_TIME:                   
			vSaveOutBackOff(1);
		break;	

		case OUT2_BACKOFF_TIME:
			vSaveOutBackOff(2);
		break;	

		case OUT3_BACKOFF_TIME:
			vSaveOutBackOff(3);
		break;	

		case OUT4_BACKOFF_TIME:
			vSaveOutBackOff(4);
		break;	
              


	}
}

void EEPROMUtils::vSaveCalib(unsigned char sensor)
{
	switch(sensor)
	{
		case 1:
			eeprom_write_block(&CalibParams, (void *)EEPROM_CALIB_STUFF, sizeof(CalibParams));
		break;
		
		case 2:
			eeprom_write_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 3:
			eeprom_write_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+2*sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 4:
			eeprom_write_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+3*sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 5:
			eeprom_write_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+4*sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 6:
			eeprom_write_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+5*sizeof(CalibParams)), sizeof(CalibParams));
		break;
	}
}

void EEPROMUtils::vReadCalib(unsigned char sensor)
{
	switch(sensor)
	{
		case 1:
			eeprom_read_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF), sizeof(CalibParams));
		break;
		
		case 2:
			eeprom_read_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 3:
			eeprom_read_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+2*sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 4:
			eeprom_read_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+3*sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 5:
			eeprom_read_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+4*sizeof(CalibParams)), sizeof(CalibParams));
		break;
		
		case 6:
			eeprom_read_block(&CalibParams, (void *)(EEPROM_CALIB_STUFF+5*sizeof(CalibParams)), sizeof(CalibParams));
		break;
	}
}
    



void EEPROMUtils::vSaveOutBackOff(unsigned char output)
{
	switch(output)
	{
		case 1:
			eeprom_write_block(&minutes2BackOffOut1, (void *)EEPROM_OUT1_BACKOFF_TIME, sizeof(unsigned char));
		break;

		case 2:
			eeprom_write_block(&minutes2BackOffOut2, (void *)(EEPROM_OUT2_BACKOFF_TIME), sizeof(unsigned char));
		break;

		case 3:
			eeprom_write_block(&minutes2BackOffOut3, (void *)(EEPROM_OUT3_BACKOFF_TIME), sizeof(unsigned char));
		break;

		case 4:
			eeprom_write_block(&minutes2BackOffOut4, (void *)(EEPROM_OUT4_BACKOFF_TIME), sizeof(unsigned char));
		break;
        
        case 5:
			eeprom_write_block(&minutes2BackOffOut5, (void *)(EEPROM_OUT5_BACKOFF_TIME), sizeof(unsigned char));
		break;
        
        case 6:
			eeprom_write_block(&minutes2BackOffOut6, (void *)(EEPROM_OUT6_BACKOFF_TIME), sizeof(unsigned char));
		break;
        
        case 7:
			eeprom_write_block(&minutes2BackOffOut7, (void *)(EEPROM_OUT7_BACKOFF_TIME), sizeof(unsigned char));
		break;
        
        case 8:
			eeprom_write_block(&minutes2BackOffOut8, (void *)(EEPROM_OUT8_BACKOFF_TIME), sizeof(unsigned char));
		break;
        
        case 9:
			eeprom_write_block(&minutes2BackOffOut9, (void *)(EEPROM_OUT9_BACKOFF_TIME), sizeof(unsigned char));
		break;
        
        case 10:
			eeprom_write_block(&minutes2BackOffOut10, (void *)(EEPROM_OUT10_BACKOFF_TIME), sizeof(unsigned char));
		break;


	}
}
    
void EEPROMUtils::vReadOutBackOff(unsigned char output)
{
	switch(output)
	{
        case 1:
            eeprom_read_block(&minutes2BackOffOut1, (void *)EEPROM_OUT1_BACKOFF_TIME, sizeof(unsigned char));
        break;

        case 2:

            eeprom_read_block(&minutes2BackOffOut2, (void *)(EEPROM_OUT2_BACKOFF_TIME), sizeof(unsigned char));
        break;

        case 3:
            eeprom_read_block(&minutes2BackOffOut3, (void *)(EEPROM_OUT3_BACKOFF_TIME), sizeof(unsigned char));
        break;

        case 4:
            eeprom_read_block(&minutes2BackOffOut4, (void *)(EEPROM_OUT4_BACKOFF_TIME), sizeof(unsigned char));
        break;            
	
        case 5:
            eeprom_read_block(&minutes2BackOffOut5, (void *)(EEPROM_OUT5_BACKOFF_TIME), sizeof(unsigned char));
        break; 
        
        case 6:
            eeprom_read_block(&minutes2BackOffOut6, (void *)(EEPROM_OUT6_BACKOFF_TIME), sizeof(unsigned char));
        break; 
        
        case 7:
            eeprom_read_block(&minutes2BackOffOut7, (void *)(EEPROM_OUT7_BACKOFF_TIME), sizeof(unsigned char));
        break; 
        
        case 8:
            eeprom_read_block(&minutes2BackOffOut8, (void *)(EEPROM_OUT8_BACKOFF_TIME), sizeof(unsigned char));
        break; 
        
        case 9:
            eeprom_read_block(&minutes2BackOffOut9, (void *)(EEPROM_OUT9_BACKOFF_TIME), sizeof(unsigned char));
        break; 
        
        case 10:
            eeprom_read_block(&minutes2BackOffOut10, (void *)(EEPROM_OUT10_BACKOFF_TIME), sizeof(unsigned char));
        break; 
        
	}
}



/****************************************************************************/