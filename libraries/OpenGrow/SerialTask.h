/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __SERIALTASK_H__
#define __SERIALTASK_H__
#include "RF24.h"

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

};

#endif // __SERIALTASK_H__
/****************************************************************************/
