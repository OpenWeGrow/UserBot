/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

//#define SPREAD_RF_CHANNELS


#include "CRC16.h"


//UserBot
//Firmware V1.0.0.1 - Base Version
//Firmware V1.0.0.2 - OpenBus implements reading of calibration factors - Mandatory!!
//Firmware V1.0.0.3 - Supports new changes to stack
//Firmware V1.0.0.4 - Serial Dump improved
//Firmware V1.0.0.5 - Ping and Errors where not sending back the correct amount of bytes
//Firmware V1.0.0.6 - Updated to latest COMS stack and cleaned to be used in UserBot public repository
//Firmware V1.0.0.7 - Code CleanUp
//Firmware v1.0.0.8 - Stack configuration changed so its configuration is a bit more simple - Cleared RS485 defines
//Firmware v1.1.0.8 - Removed Timer 1, clean up of RF24 comments, updated firmware to support HW version, added external SensorsTask state machine control, GroBot Variables clean up


#define FW_1  1
#define FW_2  1
#define FW_3  0
#define FW_4  8

//Restart after 60 seconds without any communications with a master
#define SHUTDOWN_TIMEOUT 60000

//Generic definition
#define TYPE_INPUT				0x00
#define TYPE_OUTPUT				0x01

//Possible Input Types
#define OTHER_INPUT				0x01
#define DIG_TEMPERATURE			0x02
#define WATER_TEMPERATURE		0x03
#define HUMIDITY				0x04
#define CO2						0x05
#define LIGHT					0x06
#define TANKLVL_VERT			0x07
#define TANKLVL_HORI			0x08
#define MOISTURE				0x09
#define PH						0x0A
#define EC						0x0B
#define SMOKE					0x0C
#define MOTION					0x0D
#define BUTTON					0x0E
#define BATTERY_STATE			0x0F
#define FLOOD					0x10
#define ANG_TEMPERATURE			0x11
#define POWER_MODE				0x12
#define SOIL_TEMPERATURE		0x13
#define MOISTURE_CAP			0x14
#define PRESSURE				0x15

//Possible Output Types
#define OTHER_OUTPUT			0x01
#define LIGHT_HPS				0x02
#define LIGHT_METAL_HALLID		0x03
#define VENTILATION_IN			0x04
#define VENTILATION_OUT			0x05
#define VENTILATOR				0x06
#define WATER_PUMP				0x07
#define WATER_FILTER			0x08
#define AIR_PUMP				0x09
#define ELECTROVALVE_WATER		0x0A
#define LED						0x0B
#define HUMIDIFIER				0x0C
#define DEHUMIDIFIER			0x0D
#define LIGHT_FLUO_COMPACT		0x0E
#define LIGHT_LED				0x0F
#define ELECTROVALVE_AIR		0x10
#define PERISTALTIC_PUMP		0x11
#define HEATER					0x12

//Common
#define IO_NOT_CONNECTED		0xF0

#define INPUT_INDEX0			0    //Position in inputs[0]
#define INPUT_INDEX1			1    //Position in inputs[1]
#define INPUT_INDEX2			2    //Position in inputs[2]
#define INPUT_INDEX3			3    //Position in inputs[3]
#define INPUT_INDEX4			4    //Position in inputs[4]
#define INPUT_INDEX5			5    //Position in inputs[5]
#define INPUT_INDEX6			6    //Position in inputs[6]
#define INPUT_INDEX7			7    //Position in inputs[7]
#define INPUT_INDEX8			8    //Position in inputs[8]
#define INPUT_INDEX9			9    //Position in inputs[9]

#define OUTPUT_INDEX0			0    //Position in outputs[0]
#define OUTPUT_INDEX1			1    //Position in outputs[1]
#define OUTPUT_INDEX2			2    //Position in outputs[2]
#define OUTPUT_INDEX3			3    //Position in outputs[3]
#define OUTPUT_INDEX4			4    //Position in outputs[4]
#define OUTPUT_INDEX5			5    //Position in outputs[5]
#define OUTPUT_INDEX6			6    //Position in outputs[6]
#define OUTPUT_INDEX7			7    //Position in outputs[7]
#define OUTPUT_INDEX8			8    //Position in outputs[8]
#define OUTPUT_INDEX9			9    //Position in outputs[9]

#define PERIPHERAL_INPUT		0x01
#define PERIPHERAL_OUTPUT		0x02

#define MAX_INPUTS				0x0A
#define MAX_OUTPUTS				0x0A

//Module Types
#define USERBOT					0x05

#define SET_OFF_TIMEOUT			3 

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

//State Machine External Control
extern unsigned char stMachineExternalControl;

struct stINPUT {
	unsigned char type;
	unsigned char arduinoPin;
	double value;
	char  name [15];
    unsigned long sampleTime;
};

struct stOUTPUT {
	unsigned char type;
	unsigned char arduinoPin;
	unsigned char speed;
	unsigned char value;
	char name [15];
};

extern struct stINPUT inputs[MAX_INPUTS];
extern struct stOUTPUT outputs[MAX_OUTPUTS];

//CRC Stuff
extern CRC crc;
extern unsigned char crc16 [2];

//Module Default Info
#define OPEN_DEFAULT					0x00
#define OPEN_GROWS_DEFAULT				0x00
#define OPEN_MASTERID_DEFAULT			0x01
#define OPEN_BOT_ID_DEFAULT				0xFF
#define OPEN_SLAVEADRESS_DEFAULT		0xFF
#define ZERO_MINUTES					0x00

#define DEFAULT_SERIALNUMBER_0			0x00  // Default Serial Number == 5999999999;
#define DEFAULT_SERIALNUMBER_1			0x00
#define DEFAULT_SERIALNUMBER_2			0x00

#define DEFAULT_SERIALNUMBER_3			USERBOT

#define DEFAULT_SERIALNUMBER_4			0x3B
#define DEFAULT_SERIALNUMBER_5			0x9A
#define DEFAULT_SERIALNUMBER_6			0xC9  
#define DEFAULT_SERIALNUMBER_7			0xFF

//EEPROM_SLAVE_INIT_STATE Tells if memory was written with a slave or if we are communicating first time with GroNode
#define MODULE_CONFIGED					0x01

//EEPROM Addresses of data for safe keeping data
#define EEPRROM_SLAVEADDRESS			0x00
#define EEPROM_SERIALNUMBER				0x01
#define EEPROM_SLAVE_INIT_STATE			0x09
#define EEPROM_MASTER_ID				0x0A
#define EEPROM_BOT_TYPE					0x0B
#define EEPROM_BOT_NAME					0x0C

#define EEPROM_INPUT1_START				0x1B
#define EEPROM_INPUT2_START				0x2D
#define EEPROM_INPUT3_START				0x3F
#define EEPROM_INPUT4_START				0x51
#define EEPROM_INPUT5_START				0x63
#define EEPROM_INPUT6_START				0x75
#define EEPROM_INPUT7_START				0x87
#define EEPROM_INPUT8_START				0x99
#define EEPROM_INPUT9_START				0xAB
#define EEPROM_INPUT10_START			0xBD

#define EEPROM_OUTPUT1_START			0xCF
#define EEPROM_OUTPUT2_START			0xE1
#define EEPROM_OUTPUT3_START			0xF3
#define EEPROM_OUTPUT4_START			0x105
#define EEPROM_OUTPUT5_START			0x117
#define EEPROM_OUTPUT6_START			0x129
#define EEPROM_OUTPUT7_START			0x13B
#define EEPROM_OUTPUT8_START			0x14D
#define EEPROM_OUTPUT9_START			0x15F
#define EEPROM_OUTPUT10_START			0x171

#define EEPROM_BOT_ID					0x183
#define EEPROM_CALIB_STUFF				0x184
#define FW_EEPROM_START					0x1CD

#define EEPROM_SAMPLE_TIME_INPUT1_START	0x1D1
#define EEPROM_SAMPLE_TIME_INPUT2_START	0x1D5
#define EEPROM_SAMPLE_TIME_INPUT3_START	0x1D9
#define EEPROM_SAMPLE_TIME_INPUT4_START	0x1DD

#define EEPROM_SHUTDWON_FLAG			0x1E1
#define EEPROM_SHUTDWON_TIMEOUT			0x1E2

#define EEPROM_OUT1_BACKOFF_TIME		0x1F0
#define EEPROM_OUT2_BACKOFF_TIME		0x1F1
#define EEPROM_OUT3_BACKOFF_TIME		0x1F2
#define EEPROM_OUT4_BACKOFF_TIME		0x1F3
#define EEPROM_OUT5_BACKOFF_TIME		0x1F4
#define EEPROM_OUT6_BACKOFF_TIME		0x1F5
#define EEPROM_OUT7_BACKOFF_TIME		0x1F6
#define EEPROM_OUT8_BACKOFF_TIME		0x1F7
#define EEPROM_OUT9_BACKOFF_TIME		0x1F8
#define EEPROM_OUT10_BACKOFF_TIME		0x1F9
/****************************************************************************/