/*
 Copyright (C) 2017 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __SAMPLEMETHODS_H__
#define __SAMPLEMETHODS_H__


#define TEMP_PIN          A0


void countPinChanges();


class SampleMethods
{
	private:
		

	protected:

	public:
		
		SampleMethods(void);
		int getuSAnalogRead(unsigned char sensor);
		/*#if (defined(SOILBOT_SCR) && !defined(SOILBOT_SCR_RES))|| defined(SOILBOT_SCR_BOTH)
			void pciSetup(unsigned char pin);
			void pciSetupOff(unsigned char pin);
			int countPinChanges1();
			int countPinChanges2();
			int countPinChanges3();
			int countPinChanges4();
        #endif*/
		#ifdef SOILBOT_SCR
            int getAnalogAverageValue(int index,int pin, int times2Sample);
        #endif

};

#endif // __COMSTASK_H__
/****************************************************************************/
