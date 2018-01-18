/*
*	Copyright (C) Open Grow 2014 
*	Coders: João Melo
*/

/**
 * @file RFTask.h
 *
 */

#ifndef __SERIALTASK_H__
#define __SERIALTASK_H__
#include "RF24.h"
//extern unsigned long sensorCalibValue;

/**
 * State Machine Enum
 */
extern unsigned short memadd;
 
 enum serialMachineState {
	  CHECK_SERIAL = 0,
	  GET_DATA
	};


class SerialTask
{
private:
	

protected:

public:
	
	SerialTask(void);
	void vGoSerialTask(RF24 comPort);	
	void printDisclamer();
	void reset_ParamsPH(void);
	void reset_ParamsEC(void);
    void printBotData(void);
	
};

#endif // __COMSTASK_H__
/****************************************************************************/
