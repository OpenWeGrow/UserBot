/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

//#define SPREAD_RF_CHANNELS


#include "CRC16.h"
#define RF
//#define RS485_INTERFACE


//UserBot
//Firmware V1.0.0.1 - Base Version
//Firmware V1.0.0.2 - OpenBus implements reading of calibration factors - Mandatory!!
//Firmware V1.0.0.3 - Suports new changes to stack
//Firmware V1.0.0.4 - Serial Dump improved
//Firmware V1.0.0.5 - Ping and Errors where not sending back the correct amount of bytes
//Firmware V1.0.0.6 - Updated to latest coms stack and cleanned to be used in UserBot public repository




#define FW_1  1
#define FW_2  0
#define FW_3  0
#define FW_4  6



//There are two global arrays to handle all variables data
//inputs[]/outputs[]
//Each array has a max of 10 positions
//For each sensor or output the user must define the index where it will stay in the respective table
//And also a definition of the pin that has the sensor/device connected.

//Used Inputs for the example
#define BUTTON_INDEX0   	     0    //Position in inputs[0] 
#define BUTTON_PIN0   		     2    //Pin that actually samples the button
#define BUTTON_INDEX1            1
#define BUTTON_PIN1   		     3
#define BUTTON_INDEX2   	     2
#define BUTTON_PIN2   		     4
#define TEMP_INDEX   		     3    //Position in inputs[3] 
#define TEMP_PIN   		         A0   //Pin that actually samples the sensor


//Unused Outputs
//They are not used but must be defined so the ACT_WRITE_VALUE in OpenBus can implement the backoff time
#define OUTPUT1_INDEX   	     0    //Position in inputs[0] 
#define OUTPUT2_INDEX   	     1    //Position in inputs[0] 
#define OUTPUT3_INDEX   	     2    //Position in inputs[0] 
#define OUTPUT4_INDEX   	     3    //Position in inputs[0] 
#define OUTPUT5_INDEX   	     4    //Position in inputs[0] 
#define OUTPUT6_INDEX   	     5    //Position in inputs[0] 
#define OUTPUT7_INDEX   	     6    //Position in inputs[0] 
#define OUTPUT8_INDEX   	     7    //Position in inputs[0] 
#define OUTPUT9_INDEX   	     8    //Position in inputs[0] 
#define OUTPUT10_INDEX   	     9    //Position in inputs[0] 



#define OUTPUT1_PIN      	 0   
#define OUTPUT2_PIN   	     0    
#define OUTPUT3_PIN   	     0  
#define OUTPUT4_PIN   	     0    
#define OUTPUT5_PIN   	     0    
#define OUTPUT6_PIN   	     0    
#define OUTPUT7_PIN   	     0 
#define OUTPUT8_PIN   	     0   
#define OUTPUT9_PIN   	     0   
#define OUTPUT10_PIN   	     0   


#define PERIPHERAL_INPUT   0x01
#define PERIPHERAL_OUTPUT  0x02

#define MAX_INPUTS 0x0A
#define MAX_OUTPUTS 0x0A


//Module Types
#define USERBOT                 0x05

#define SET_OFF_TIMEOUT   3 
//Debug the code as you wish, carefull can cause memory full!!

//#define DEBUGCOMSTASK     //Implements debug prinf on COMSTAK
//#define DEBUGOPENBUS      //Implements debug prinf on COMSTAK
//#define DEBUGRF         //Implements debug prinf on RFCOMS
//#define GENERALDEBUG         //Implements debug prinf on RFCOMS
//#define DEBUGBASIC

//Module Config Variables
extern unsigned char slaveAddress;
extern unsigned char serialNumber[8];
extern char name[15];
extern unsigned char masterID;
extern unsigned char botType;
extern unsigned char botID;



//Safety Shutdown variables
extern unsigned char need2Shutdown;
extern unsigned long timeout2Shutdown;
extern unsigned char shutdownFlag;
extern unsigned long shutdownTimeout;
#define SHUTDOWN_TIMEOUT 60000

    
struct stINPUT {
	unsigned char type;
	unsigned char growRoomID;
	unsigned char growID;
	double value;
	char  name [15];
    unsigned long sampleTime;
};

struct stOUTPUT {
	unsigned char type;
	unsigned char growRoomID;
	unsigned char growID;
	unsigned char speed;
	unsigned char value;
	char name [15];
};

extern struct stINPUT inputs[MAX_INPUTS];
extern struct stOUTPUT outputs[MAX_OUTPUTS];

//CRC Stuff
extern CRC crc;
extern unsigned char crc16 [2];

   
enum sensorsMachineState {
  INIT_SENSORS = 0,
  GET_TEMP,	
  GET_IOS,
  PRINT_OUT
};
extern sensorsMachineState snsState;
//Module Default Info
#define OPEN_DEFAULT 0x00
#define OPEN_GROWS_DEFAULT 0x00
#define OPEN_MASTERID_DEFAULT 0x01
#define OPEN_BOT_ID_DEFAULT	  0xFF
#define OPEN_SLAVEADRESS_DEFAULT	  0xFF
#define ZERO_MINUTES 0x00

#define DEFAULT_SERIALNUMBER_0 0x00  // Default Serial Number == 5999999999;
#define DEFAULT_SERIALNUMBER_1 0x00
#define DEFAULT_SERIALNUMBER_2 0x00


#define DEFAULT_SERIALNUMBER_3 USERBOT



#define DEFAULT_SERIALNUMBER_4 0x3B
#define DEFAULT_SERIALNUMBER_5 0x9A
#define DEFAULT_SERIALNUMBER_6 0xC9  
#define DEFAULT_SERIALNUMBER_7 0xFF

//EEPROM_SLAVE_INIT_STATE Tells if memory was written with a slave or if we are comunicating first time with GroNode
#define MODULE_CONFIGED  0x01

//EEPROM Addresses of data for safe keeping data
#define EEPRROM_SLAVEADDRESS 		0x00
#define EEPROM_SERIALNUMBER 		0x01
#define EEPROM_SLAVE_INIT_STATE 	0x09
#define EEPROM_MASTER_ID        	0x0A
#define EEPROM_BOT_TYPE      		0x0B
#define EEPROM_BOT_NAME      		0x0C

#define EEPROM_INPUT1_START			0x1B
#define EEPROM_INPUT2_START 		0x2D
#define EEPROM_INPUT3_START 		0x3F
#define EEPROM_INPUT4_START 		0x51
#define EEPROM_INPUT5_START 		0x63
#define EEPROM_INPUT6_START 		0x75
#define EEPROM_INPUT7_START 		0x87
#define EEPROM_INPUT8_START 		0x99
#define EEPROM_INPUT9_START 		0xAB
#define EEPROM_INPUT10_START 		0xBD

#define EEPROM_OUTPUT1_START 		0xCF
#define EEPROM_OUTPUT2_START 		0xE1
#define EEPROM_OUTPUT3_START 		0xF3
#define EEPROM_OUTPUT4_START 		0x105
#define EEPROM_OUTPUT5_START 		0x117
#define EEPROM_OUTPUT6_START 		0x129
#define EEPROM_OUTPUT7_START 		0x13B
#define EEPROM_OUTPUT8_START 		0x14D
#define EEPROM_OUTPUT9_START 		0x15F
#define EEPROM_OUTPUT10_START 		0x171

#define EEPROM_BOT_ID 			    0x183
#define EEPROM_CALIB_STUFF   		0x184
#define FW_EEPROM_START   		    0x1CD

#define EEPROM_SAMPLE_TIME_INPUT1_START  0x1D1
#define EEPROM_SAMPLE_TIME_INPUT2_START  0x1D5
#define EEPROM_SAMPLE_TIME_INPUT3_START  0x1D9
#define EEPROM_SAMPLE_TIME_INPUT4_START  0x1DD
    
#define EEPROM_SHUTDWON_FLAG             0x1E1
#define EEPROM_SHUTDWON_TIMEOUT          0x1E2

#define EEPROM_OUT1_BACKOFF_TIME         0x1F0
#define EEPROM_OUT2_BACKOFF_TIME         0x1F1
#define EEPROM_OUT3_BACKOFF_TIME         0x1F2
#define EEPROM_OUT4_BACKOFF_TIME         0x1F3
#define EEPROM_OUT5_BACKOFF_TIME         0x1F4
#define EEPROM_OUT6_BACKOFF_TIME         0x1F5
#define EEPROM_OUT7_BACKOFF_TIME         0x1F6
#define EEPROM_OUT8_BACKOFF_TIME         0x1F7
#define EEPROM_OUT9_BACKOFF_TIME         0x1F8
#define EEPROM_OUT10_BACKOFF_TIME        0x1F9  
    

//Generic definition
#define TYPE_INPUT      	  		0x00
#define TYPE_OUTPUT 				0x01
//Possible Input Peripherics
#define OTHER_INPUT         0x01
#define DIG_TEMPERATURE   	0x02
#define WATER_TEMPERATURE   0x03
#define HUMIDITY      		0x04
#define CO2      	  		0x05
#define LIGHT         		0x06
#define TANKLVL_VERT  		0x07
#define TANKLVL_HORI   		0x08
#define MOISTURE    		0x09
#define PH      	  		0x0A
#define EC    		  		0x0B
#define SMOKE      	  		0x0C
#define MOTION        		0x0D
#define BUTTON     	  		0x0E
#define BATTERY_STATE 		0x0F
#define FLOOD	    		0x10
#define ANG_TEMPERATURE   	0x11
#define POWER_MODE       	0x12
#define SOIL_TEMPERATURE    0x13
#define MOISTURE_CAP        0x14
//Possible Status Still and Input

//Possible Output Peripherics
#define OTHER_OUTPUT        0x01
#define LIGHT_VEG_LOW       0x02
#define LIGHT_FLO_LOW       0x03
#define VENTILATION_IN      0x04
#define VENTILATION_OUT     0x05
#define VENTILATOR          0x06
#define WATER_PUMP          0x07
#define WATER_FILTER        0x08
#define AIR_PUMP            0x09
#define ELECTROVALVE        0x0A
#define LED                 0x0B
#define HUMIDIFIER          0x0C
#define DEHUMIDIFIER        0x0D
#define LIGHT_VEG_HPS       0x0E
#define LIGHT_FLO_HPS       0x0F
//
//Common 
#define IO_NOT_CONNECTED 0xF0

/****************************************************************************/

