/*
*	Copyright (C) Open Grow 2014 
*	Coders: João Melo
*/

/**
 * @file OpenBus.h
 *
 */

#ifndef __OPENBUS_H__
#define __OPENBUS_H__


//Uncomment only when using Serial Port for RS-485
//#define RS485

#if defined (RF)
	#include "RF24.h"	
#endif

#if defined(RS485_INTERFACE)
	#include "RS485.h"	
#endif

//Calib Types - Used in SOILBOT for moisture and TankBot for PH and EC
#define CALIB_HIGH     0x01
#define CALIB_LOW      0x02
#define CALIB_OUTPUT   0x03
#define CALIB_BACKOFF  0x04

//Frame Communication Types
#define OPENINPUT   0x01
#define OPENOUTPUT  0x02
#define OPENCONFIG  0x03

//Frame Communication Types
#define ACT_READ_VALUE        				0x01
#define ACT_WRITE_VALUE       				0x02 //It's only for outputs
#define ACT_READ_GROWROOMID   				0x03
#define ACT_WRITE_GROWROOMID  				0x04
#define ACT_READ_GROWID       				0x05
#define ACT_WRITE_GROWID      				0x06
#define ACT_READ_ALL_INOUT_VALUES   		0x07
#define ACT_WRITE_SPEED			    		0x08
#define ACT_WRITE_NAME			    		0x09
#define ACT_WRITE_SLAVEADRESS        		0x0A
#define ACT_WRITE_SERIALNUMBER        		0x0B //Production Only
#define ACT_READ_TYPE                       0x0C
#define ACT_WRITE_TYPE                      0x0D 
#define ACT_READ_IO_NAME                    0x0E
//#define ACT_WRITE_IO_NAME                   0x0F
#define ACT_CALIB                           0x11
#define ACT_MASTERID                        0x12
#define ACT_CALIB_READ                      0x13
#define ACT_WRITE_MOIST_SAMPLE_TIME         0x14
#define ACT_READ_MOIST_SAMPLE_TIME          0x15
#define ACT_SET_SHUTDOWN                    0x16
#define ACT_GET_SHUTDOWN                    0x17
#define ACT_SET_BACKOFF                     0x18
#define ACT_GET_BACKOFF                     0x19
#define ACT_READ_SPEED                      0x1A  //Only from V1.8.0.8 - TankBot     V 1.2.0.8 - PowerBot

#define ACT_PING			    			0xC9
#define ACT_FORCE_EEPROM_SAVE               0xCA
#define ACT_FORCE_AUTHENTICATION            0xCB
#define ACT_AUTHENTICATION          		0xCC
#define ACT_AUTHENTICATION_MASTER_RESPONSE  0xCD


// OpenBus exception codes
		/**
			OpenBus protocol illegal function exception.   
		*/		
	
		


		const unsigned char ucOBSuccess              = 0xCC;
        
		const unsigned char ucOBError                = 0xDD;
        const unsigned char ucOBBackOff              = 0xE6;
		
		const unsigned char ucOBAuthNew              = 0xA0;
		const unsigned char ucOBAuthExists           = 0xA1;
		const unsigned char ucOBAuthOk               = 0xA2;
		

		const unsigned char ucOBInvalidCRC           = 0xE3;

		

	
		
class OpenBus
{
	private:

	protected:

	public:		
		
		OpenBus();
		#ifdef RF
			unsigned char usOpenBusReply(unsigned char * dataIn, unsigned char * dataOut,RF24 comPort, unsigned char channel);
			unsigned char usOpenBusCreateAuthFrame(unsigned char * dataOut,RF24 comPort, unsigned char channel);
		#endif
		#ifdef RS485_INTERFACE
			unsigned char  usOpenBusReply(unsigned char * dataIn, unsigned char * dataOut,RS485  comPort);
			unsigned char usOpenBusCreateAuthFrame(unsigned char * dataOut,RS485 comPort);
		#endif		

		#if defined(DEBUGOPENBUS) || defined(DEBUGOPENBUS_EXTRA)
			void printDouble(double val, unsigned char precision);
		#endif
};

#endif // __OPENBUS_H__
/****************************************************************************/