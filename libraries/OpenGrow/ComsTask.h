/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __COMSTASK_H__
#define __COMSTASK_H__

#include "RF24.h"	
#include "EEPROM_Utils.h"

#define AUTH_CHANNEL  0x00
#define MASTER_WRITE_CHANNEL 0x01
#define MASTER_READ_CHANNEL  0x02

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

extern unsigned char shutdownFlag; //Default shutdown ON
extern unsigned long shutdownTimeout; //Default waiting time 1 minute

void saveModuleConfig(void);

class ComsTask
{
	private:
		unsigned char data [256];

	protected:

	public:
		ComsTask(void);
		void(* resetFuncCom) (void) = 0; //declare reset function @ address 0
		void vGoComsTask(RF24 comPort);
};

#endif // __COMSTASK_H__
/****************************************************************************/