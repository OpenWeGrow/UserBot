/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef EEPROM_UTILS_h
#define EEPROM_UTILS_h

#define SAVE_SLAVEADRESS				0x02
#define SAVE_SERIALNUMBER				0x05
#define SAVE_MASTERID					0x06
#define SAVE_INITSTATE					0x08

#define CALIB_INPUT						0x1D
#define CALIB_INPUT_1   				0x1D
#define CALIB_INPUT_2    				0x1E
#define CALIB_INPUT_3   				0x1F
#define CALIB_INPUT_4   				0x20
#define CALIB_INPUT_5   				0x21
#define CALIB_INPUT_6   				0x22
#define CALIB_INPUT_7   				0x23
#define CALIB_INPUT_8   				0x24
#define CALIB_INPUT_9   				0x25
#define CALIB_INPUT_10   				0x26

#define SHUTDOWN_CONFIG  				0x27

#define OUT1_BACKOFF_TIME  				0x28
#define OUT2_BACKOFF_TIME  				0x29
#define OUT3_BACKOFF_TIME  				0x2A
#define OUT4_BACKOFF_TIME  				0x2B
#define OUT5_BACKOFF_TIME  				0x2C
#define OUT6_BACKOFF_TIME  				0x2D
#define OUT7_BACKOFF_TIME  				0x2E
#define OUT8_BACKOFF_TIME  				0x2F
#define OUT9_BACKOFF_TIME  				0x30
#define OUT10_BACKOFF_TIME				0x31

#define OUT_SPEED       				0x32
#define OUT1_SPEED       				0x32
#define OUT2_SPEED       				0x33
#define OUT3_SPEED       				0x34
#define OUT4_SPEED       				0x35
#define OUT5_SPEED       				0x36
#define OUT6_SPEED       				0x37
#define OUT7_SPEED       				0x38
#define OUT8_SPEED       				0x39
#define OUT9_SPEED       				0x3A
#define OUT10_SPEED       				0x3B

#define HIGHER_VERSION   0x01
#define LOWER_VERSION    0x02
#define EQUAL_VERSION    0x03
#define ERROR_VERSION    0x04


#define MILLIS_PER_MINUTE 60000

struct parameters_Moisture
{
	//unsigned long time;
	unsigned long timeMax;
	unsigned long timeMin;
	float m;
};

extern parameters_Moisture CalibParams;

extern unsigned char minutes2BackOffOut1;
extern unsigned char minutes2BackOffOut2;
extern unsigned char minutes2BackOffOut3;
extern unsigned char minutes2BackOffOut4;
extern unsigned char minutes2BackOffOut5;
extern unsigned char minutes2BackOffOut6;
extern unsigned char minutes2BackOffOut7;
extern unsigned char minutes2BackOffOut8;
extern unsigned char minutes2BackOffOut9;
extern unsigned char minutes2BackOffOut10;

extern unsigned char minutes2BackOffOut1;
extern unsigned char minutes2BackOffOut2;
extern unsigned char minutes2BackOffOut3;
extern unsigned char minutes2BackOffOut4;
extern unsigned char minutes2BackOffOut5;
extern unsigned char minutes2BackOffOut6;
extern unsigned char minutes2BackOffOut7;
extern unsigned char minutes2BackOffOut8;
extern unsigned char minutes2BackOffOut9;
extern unsigned char minutes2BackOffOut10;

class EEPROMUtils
{
	public:
		EEPROMUtils();
        unsigned char checkVersion(unsigned char * currentFWVersion, unsigned char * version2Check);
		void vSaveSlaveAdress(unsigned char newSlaveAdress);
		void vSaveBotID(unsigned char newBotID);
		void saveModuleConfig(unsigned char saveWhat);
		void resetGroBot2Defaults();
		void vSaveCalib(unsigned char sensor);
		void vReadCalib(unsigned char sensor);
		void vSaveShutdown();
		void vSaveOutBackOff(unsigned char output);
		void vReadOutBackOff(unsigned char output);
        void vSaveSpeed(unsigned char output);
		void vReadSpeed(unsigned char output);
		void dumpEEPROM ();
};
#endif //EEPROM_UTILS_h
/****************************************************************************/