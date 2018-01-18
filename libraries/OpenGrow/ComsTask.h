/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __COMSTASK_H__
#define __COMSTASK_H__



#if defined (RF)
	#include "RF24.h"	
#endif

#if defined(RS485_INTERFACE)
	#include "RS485.h"	
#endif

#include "EEPROM_Utils.h"
#include "SampleMethods.h"

#define AUTH_CHANNEL  0x00
#define MASTER_WRITE_CHANNEL 0x01
#define MASTER_READ_CHANNEL  0x02



extern SampleMethods vSampleMethods;

/**
 * State Machine Enum
 */
 extern unsigned short memadd;
 enum machineState {
	  INIT = 0,
	  SEARCH_MASTER,
	  GET_COMMAND
	};

extern machineState mState;
extern EEPROMUtils vEEPROMUtils;
extern unsigned char slaveAddress;
extern unsigned char fwVersion [4];
extern unsigned long ticksOut1;
extern unsigned long ticksOut2;
extern unsigned long ticksOut3;
extern unsigned long ticksOut4;
extern unsigned long ticksOut5;
extern unsigned long ticksOut6;
extern unsigned long ticksOut7;
extern unsigned long ticksOut8;
extern unsigned long ticksOut9;
extern unsigned long ticksOut10;


void saveModuleConfig(void);

class ComsTask
{
private:
	
	unsigned char data [256];	

protected:

public:
	
	ComsTask(void);
	void(* resetFuncCom) (void) = 0; //declare reset function @ address 0
    
    #if defined (RF)
        void vGoComsTask(RF24 comPort);
    #endif

    #if defined(RS485_INTERFACE)
        void vGoComsTask(RS485 comPort);
    #endif
	
	
};

#endif // __COMSTASK_H__
/****************************************************************************/
