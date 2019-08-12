/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
#include "GroBot_Variables.h"
#include "RF24.h"
#include "OpenBus.h"
#include "Arduino.h"
#include <EEPROM.h>
#include "ComsTask.h"

unsigned char dataInLenght = 0;
unsigned char dataOutLenght = 0;
unsigned char validAction=false;
unsigned char saveModule = 0x00;
unsigned char areWeYourDroid = 0x00;
unsigned char per = 0x00;
unsigned char per_type = 0x00;
bool comsOk = false;
unsigned char failCount = 0;

//Iteration stuff
unsigned short oB;

//External Variables
extern unsigned char slaveAddress;
extern unsigned char serialNumber[8];
extern char name[15];
extern unsigned char masterID;
extern unsigned char botType;
extern unsigned short memadd;
extern unsigned char botID;
extern unsigned char reboot;

OpenBus::OpenBus()
{

}

unsigned char OpenBus::usOpenBusReply(unsigned char * dataIn, unsigned char * dataOut,RF24 comPort, unsigned char channel)
{
    CRC crc;
    unsigned char crc16 [2];
    unsigned char outLastValue;
    bool resetTicksBackoff = false;
    unsigned long ticksToEndBackOff = 0;
    bool validOnOff = false;
    unsigned char minutes2Backoff;
    unsigned char aid; 

	comsOk = comPort.read_Frame(dataIn,channel); //Data sent ok so receive new frame
	failCount = 0;

	if(comsOk)
	{
		validAction = false;
		dataInLenght = dataIn[1]; //Length of Header (SlaveAdress + DataSizeBytes + Data)
		crc.CRC16(dataIn,dataInLenght,crc16); //Check CRC of data

        if(!(dataIn[dataInLenght] == crc16[0] && dataIn[dataInLenght+1] == crc16[1])) //Check for CRC
		{
            unsigned short shift = 0;
            for(shift = 0; shift < 255; shift++)
            {
                if(dataIn[shift] == slaveAddress)
                {
                    break;
                }
            }
            memset(dataOut,0x00,sizeof(dataOut));
            memcpy(dataOut,dataIn+shift,(255 - shift));
            memcpy(dataIn,dataOut,sizeof(255));
            memset(dataOut,0x00,sizeof(dataOut));
            Serial.println("CRC1");
        }
		
        //Now verify again the CRC
        dataInLenght = dataIn[1]; //Length of Header (SlaveAdress + DataSizeBytes + Data)
        crc.CRC16 (dataIn,dataInLenght,crc16); //Check CRC of data
		if(dataIn[dataInLenght] == crc16[0] && dataIn[dataInLenght+1] == crc16[1]) //Check for CRC
		{
			#ifdef DEBUGBASIC
				Serial.println("CRC OK");
			#endif
			areWeYourDroid = 0x01; //Assume that we are the droid the master is looking for, if not don't send any response
			if(slaveAddress == dataIn[0] || (dataIn[0] == 0xFF && dataIn[3] == ACT_AUTHENTICATION_MASTER_RESPONSE))
			{
				dataOutLenght=0;
				#ifdef DEBUGBASIC
					Serial.println("CMD OK");
				#endif
				
				if(dataIn[3] != ACT_AUTHENTICATION_MASTER_RESPONSE)
				{
					dataInLenght = 4;  //First byte of concrete data to read
					dataOut[dataOutLenght++] = masterID;
					dataOut[dataOutLenght++] = 0xFF;
					dataOut[dataOutLenght++] = slaveAddress;
				}
				else
				{
					dataInLenght = dataIn[1]; //We need to know data size to handle authentication	
											  //Also slave address and master ID
				}
				
				switch(dataIn[3])
				{
					case ACT_FORCE_AUTHENTICATION:
						break;
						
                    case ACT_MASTERID:
                        reboot = dataIn[4];
                        dataOut[dataOutLenght++] = ACT_MASTERID;
                        dataOut[dataOutLenght++] = ucOBSuccess;
                        dataOut[dataOutLenght++] = ucOBSuccess;
                        dataOut[dataOutLenght++] = reboot;	//New masterID                                
                        validAction = true;
						break;

                    case ACT_SET_SHUTDOWN:
                        shutdownFlag = dataIn[4];
                        memcpy(&shutdownTimeout,dataIn+5,sizeof(shutdownTimeout));
                        dataOut[dataOutLenght++]=ACT_SET_SHUTDOWN;
                        dataOut[dataOutLenght++]=ucOBSuccess;
                        dataOut[dataOutLenght++]=ucOBSuccess;
                        validAction =true;
                        saveModule = SHUTDOWN_CONFIG;
						break;
                        
                    case ACT_GET_SHUTDOWN:
						dataOut[dataOutLenght++]=ACT_GET_SHUTDOWN;
						dataOut[dataOutLenght++]=ucOBSuccess;  
                        dataOut[dataOutLenght++]=ucOBSuccess; 
                        dataOut[dataOutLenght++]=shutdownFlag;  
						memcpy(dataOut+dataOutLenght,&shutdownTimeout,sizeof(shutdownTimeout));
						dataOutLenght+= sizeof(shutdownTimeout);
						validAction = true;
						saveModule = 0x00; 
						break;

					case ACT_AUTHENTICATION_MASTER_RESPONSE:
							if(dataIn[4] == ucOBAuthExists)
							{
								if(memcmp(dataIn+dataInLenght-23,serialNumber,8)==0)// check if serialNumber matches ours
								{
									slaveAddress = dataIn[5]; //Get new slave address
									botID = dataIn[6];
									masterID = dataIn[2];									
									oB=0;
									dataInLenght = 8;
									//ReWrite our inputs and outputs tables, in case master sent us different configs
									//For now it's always the same config sent
									
									while(dataIn[dataInLenght]!=0xFF)
									{
										oB = dataIn[dataInLenght+1];
										inputs[oB].type = dataIn[dataInLenght++];
										dataInLenght++;
										dataInLenght++;
										dataInLenght++;
										//inputs[oB].growRoomID = dataIn[dataInLenght++];
										//inputs[oB].growID = dataIn[dataInLenght++];
                                        #ifdef SOILBOT_SCR
                                            memcpy(&inputs[oB].sampleTime, dataOut+dataInLenght,sizeof(unsigned long));
                                            dataInLenght+=4;
                                        #endif
									}

									dataInLenght++;

									oB=0;
									while(dataIn[dataInLenght]!=0xFF)
									{
										oB = dataIn[dataInLenght + 1];
										outputs[oB].type = dataIn[dataInLenght++];
										dataInLenght++;
										dataInLenght++;
										dataInLenght++;
										//outputs[oB].growRoomID = dataIn[dataInLenght++];
										//outputs[oB].growID = dataIn[dataInLenght++];
									}
									dataInLenght++;
									dataInLenght+=8;	//Discard Serial Number

									for(oB=0;oB<15;oB++)
										name[oB]=dataIn[dataInLenght++];


									dataOut[dataOutLenght++]=masterID;
									dataOut[dataOutLenght++]=0xFF;
									dataOut[dataOutLenght++]=slaveAddress;
									dataOut[dataOutLenght++]=ACT_AUTHENTICATION_MASTER_RESPONSE;
									dataOut[dataOutLenght++]=ucOBSuccess;
									dataOut[dataOutLenght++]=ucOBAuthOk;

									validAction=true;								
								}
								else
								{
									areWeYourDroid = 0x00;
								}
							}
							else if (dataIn[4] == ucOBAuthNew)
							{       
								slaveAddress = dataIn[5]; //Get new slave address
								masterID = dataIn[2];
								botID = dataIn[6];

								vEEPROMUtils.vSaveSlaveAdress(slaveAddress);
								vEEPROMUtils.vSaveBotID(botID);
								dataOut[dataOutLenght++]=masterID;
								dataOut[dataOutLenght++]=0xFF;
								dataOut[dataOutLenght++]=slaveAddress;
								dataOut[dataOutLenght++]=ACT_AUTHENTICATION_MASTER_RESPONSE;
								dataOut[dataOutLenght++]=ucOBSuccess;
								dataOut[dataOutLenght++]=ucOBAuthOk;
								validAction=true;
							}
						break;

					case ACT_READ_VALUE:
						per = dataIn[5];
						per_type = dataIn[4];

						dataOut[dataOutLenght++] = ACT_READ_VALUE;

						if(per < MAX_INPUTS)
						{
							if(per_type == PERIPHERAL_INPUT && inputs[per].type != 0x00)
							{
								dataOut[dataOutLenght++] = ucOBSuccess;	
								dataOut[dataOutLenght++] = per_type;
								dataOut[dataOutLenght++] = per;							
								memcpy(dataOut + dataOutLenght, &inputs[per].value, sizeof(double));						
								dataOutLenght += 4;
								validAction = true;	
							}
							else if(per_type == PERIPHERAL_OUTPUT && outputs[per].type != 0x00)
							{
								dataOut[dataOutLenght++] = ucOBSuccess;
								dataOut[dataOutLenght++] = per_type;
								dataOut[dataOutLenght++] = per;
								dataOut[dataOutLenght++] = outputs[per].value;
								validAction = true;
							}
							else
							{                           
								dataOut[dataOutLenght++]=ucOBError;
								validAction = true;	
							}
						}
						else
						{
							dataOut[dataOutLenght++]=ucOBError;
							validAction = true;	
						}
							
						break;

					case ACT_READ_IO_NAME:				
						dataOut[dataOutLenght++] = ACT_READ_IO_NAME;

						if(dataIn[5] < MAX_INPUTS)
						{
							per = dataIn[5];
							per_type = dataIn[4];
							
							if(per_type == PERIPHERAL_INPUT && inputs[per].type != 0x00)
							{
								dataOut[dataOutLenght++]=ucOBSuccess;
								dataOut[dataOutLenght++]=per_type;
								dataOut[dataOutLenght++]=per;
								memcpy(dataOut+dataOutLenght,&inputs[per].name,sizeof(inputs[per].name));
								dataOutLenght+=15;
								validAction = true;
							}
							else if(per_type == PERIPHERAL_OUTPUT && outputs[dataIn[5]].type != 0x00)
							{
								dataOut[dataOutLenght++]=ucOBSuccess;
								dataOut[dataOutLenght++]=per_type;
								dataOut[dataOutLenght++]=per;
								memcpy(dataOut+dataOutLenght,&outputs[per].name,sizeof(outputs[per].name));
								dataOutLenght+=15;
								validAction = true;
							}
							else
							{
								dataOut[dataOutLenght++]=ucOBError;
								validAction = true;
							}
						}
						else
						{
							dataOut[dataOutLenght++] = ucOBError;
							validAction = true;
						}
						break;				

					case ACT_WRITE_VALUE:
						dataOut[dataOutLenght++]=ACT_WRITE_VALUE;
						if(dataIn[5]<MAX_INPUTS)
						{
							per = dataIn[5];
							per_type = dataIn[4];

							if(per_type == PERIPHERAL_OUTPUT && outputs[per].type != 0x00)
							{
								outLastValue = outputs[per].value;
								outputs[per].value = dataIn[6];

								if(dataIn[7]==0x00)
								{
									//Check if we need to back off in case of ON action
									switch(per)
									{                                       
										case OUTPUT_INDEX0:
											aid = outputs[OUTPUT_INDEX0].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut1;                                                           

											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut1) )
											{
												//Serial.println("Reset Ticks");
												resetTicksBackoff = true;
											}
											else
											{
												//Serial.println("No Tick Reset");
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX1:
											aid = outputs[OUTPUT_INDEX1].arduinoPin;                                              
											ticksToEndBackOff = millis() - ticksOut2;
											
											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut2) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX2:
											aid = outputs[OUTPUT_INDEX2].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut3;
											
											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut3) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX3:
											aid = outputs[OUTPUT_INDEX3].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut4; 

											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut4) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX4:
											aid = outputs[OUTPUT_INDEX4].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut5; 

											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut5) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX5:
											aid = outputs[OUTPUT_INDEX5].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut6; 
											
											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut6) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX6:
											aid = outputs[OUTPUT_INDEX6].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut7; 

											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut7) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX7:
											aid = outputs[OUTPUT_INDEX7].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut8; 

											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut8) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX8:
											aid = outputs[OUTPUT_INDEX8].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut9; 

											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut9) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;

										case OUTPUT_INDEX9:
											aid = outputs[OUTPUT_INDEX9].arduinoPin;  
											ticksToEndBackOff = millis() - ticksOut10; 
											if(ticksToEndBackOff > (MILLIS_PER_MINUTE * minutes2BackOffOut10) )
											{
												resetTicksBackoff = true;
											}
											else
											{
												resetTicksBackoff = false;
											}
											break;
									}
								}
								else
								{
									resetTicksBackoff = true;  
									//Serial.println("Force On");
								}
								//If ON, see the validation of the timers
								if(outputs[per].value>0)
								{
									//Serial.println("Output was on");
									if(resetTicksBackoff)
									{
										switch(per)
										{
											case OUTPUT_INDEX0:
												ticksOut1 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut1;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break; 

											case OUTPUT_INDEX1:
												ticksOut2 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut2;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break;  

											case OUTPUT_INDEX2:
												ticksOut3 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut3;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break; 

											case OUTPUT_INDEX3:
												ticksOut4 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut4;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break; 

											case OUTPUT_INDEX4:
												ticksOut5 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut5;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break;  

											case OUTPUT_INDEX5:
												ticksOut6 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut6;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break;  

											case OUTPUT_INDEX6:
												ticksOut7 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut7;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break;  

											case OUTPUT_INDEX7:
												ticksOut8 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut8;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break; 

											case OUTPUT_INDEX8:
												ticksOut9 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut9;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;   
												break;  

											case OUTPUT_INDEX9:
												ticksOut10 = millis() + MILLIS_PER_MINUTE * minutes2BackOffOut10;
												aid = outputs[OUTPUT_INDEX0].arduinoPin;  
												break; 
										}

									   outputs[per].value = 0xFF;
									   validOnOff = true;
									}
									else
									{
										Serial.println("BF");
										outputs[per].value = 0;
										validOnOff = false;
									}
								}
								else
								{
									//If the output was ON, we reset the timers
									if(outLastValue > 0)
									{
										switch(per)
										{
											case OUTPUT_INDEX0:
												ticksOut1 = millis();
												break;
										   case OUTPUT_INDEX1:
												ticksOut2 = millis();
												break;
											case OUTPUT_INDEX2:
												ticksOut3 = millis();
												break;
											case OUTPUT_INDEX3:
												ticksOut4 = millis();
												break;
											case OUTPUT_INDEX4:
												ticksOut5 = millis();
												break;
											case OUTPUT_INDEX5:
												ticksOut6 = millis();
												break;
											case OUTPUT_INDEX6:
												ticksOut7 = millis();
												break;
											case OUTPUT_INDEX7:
												ticksOut8 = millis();
												break;
											case OUTPUT_INDEX8:
												ticksOut9 = millis();
												break;
											case OUTPUT_INDEX9:
												ticksOut10 = millis();
												break;
										}
									}                                    

									digitalWrite(aid, LOW);
									validOnOff = true;
								}

								//Force sensors task to act on outputs
								stMachineExternalControl = 0x01;
								//snsState =  ACT_ON_IOS;

								if(validOnOff)
								{
									dataOut[dataOutLenght++] = ucOBSuccess;
								}
								else
								{
									dataOut[dataOutLenght++] = ucOBBackOff;
								}

								dataOut[dataOutLenght++] = per_type;
								dataOut[dataOutLenght++] = per;

								if(!validOnOff)
								{
									memcpy(dataOut + dataOutLenght, &ticksToEndBackOff, sizeof(ticksToEndBackOff));
									dataOutLenght += sizeof(ticksToEndBackOff);
								}
								validAction = true;

							}
							else
							{
								dataOut[dataOutLenght++] = ucOBError;
								validAction = true;
							}
						}
						else
						{
							dataOut[dataOutLenght++] = ucOBError;
							validAction = true;
						}
						break;

					case ACT_WRITE_SPEED:				
                        dataOut[dataOutLenght++]=ACT_WRITE_SPEED;

                        if(dataIn[5]<MAX_INPUTS)
                        {
                            per = dataIn[5];
                            per_type = dataIn[4];
                            
                            if(per_type==PERIPHERAL_OUTPUT)
                            {	
                                vEEPROMUtils.vReadCalib(per+3);  
                                
                                if(outputs[per].speed != dataIn[6])
                                {
                                    outputs[per].speed = dataIn[6];
                                    CalibParams.timeMax = outputs[per].speed;   
                                    vEEPROMUtils.vSaveCalib(per+3);
                                }
                                dataOut[dataOutLenght++]=ucOBSuccess;
                                dataOut[dataOutLenght++]=per_type;
                                dataOut[dataOutLenght++]=per;
                                validAction = true;							
                            }
                            else
                            {
                                dataOut[dataOutLenght++]=ucOBError;
                                validAction = true;	
                            }
                        }
                        else
                        {
                            dataOut[dataOutLenght++]=ucOBError;
                            validAction = true;	
                        }
						break;
                    
                    case ACT_READ_SPEED:				
						dataOut[dataOutLenght++]=ACT_READ_SPEED;

						if(dataIn[5]<MAX_INPUTS)
						{
							per = dataIn[5];
							per_type = dataIn[4];

							if(per_type==PERIPHERAL_OUTPUT /*&& outputs[per].type != 0x00*/)
							{	
								vEEPROMUtils.vReadCalib(per+3);
								outputs[per].speed = CalibParams.timeMax;
								dataOut[dataOutLenght++]=ucOBSuccess;	
								dataOut[dataOutLenght++]=per_type;
								dataOut[dataOutLenght++]=per;	
								dataOut[dataOutLenght++]= outputs[per].speed;
								validAction = true;							
							}
							else
							{
								dataOut[dataOutLenght++]=ucOBError;
								validAction = true;	
							}
						}
						else
						{
							dataOut[dataOutLenght++]=ucOBError;
							validAction = true;	
						}
						break;
					
					case ACT_CALIB:
						dataOut[dataOutLenght++] = ACT_CALIB;

						per = dataIn[5];      //Calib Input Index
						per_type = dataIn[4]; //Type of Calib High or Low

						if(per<7) //6 is the max number of soil moisture sensors
						{
							vEEPROMUtils.vReadCalib(per+1);	
							if(per_type == CALIB_HIGH)
							{
								//Do your calib sampling for a high alue then save it on  CalibParams.timeMax
								//After that you need to call saveModule= CALIB_INPUT + per;
								//This will ensure the value you just got gets saved in EEPROM    
							   
							}
							else if(per_type == CALIB_LOW)
							{
								//Do your calib sampling for a high alue then save it on  CalibParams.timeMax
								//After that you need to call saveModule= CALIB_INPUT + per;
								//This will ensure the value you just got gets saved in EEPROM                            
							  
							}
							else if(per_type == CALIB_OUTPUT)
							{
								//Receive a full structure of calibration data instead a high and a low seperated
								CalibParams.timeMin =  dataIn[6];      //Calib Input Index
								CalibParams.timeMax =  dataIn[7];      //Calib Input Index
								memcpy(&CalibParams.m,dataIn+8,sizeof(float));                  
							}

							//Print to the UART the calibration parameters
							Serial.print("Max: ");
							Serial.print(CalibParams.timeMax);
							Serial.print("  Min: ");
							Serial.print(CalibParams.timeMin);
							Serial.print("  M: ");
							Serial.print(CalibParams.m);
							Serial.println(" %");

							dataOut[dataOutLenght++] = ucOBSuccess;
							memcpy(dataOut + dataOutLenght, &CalibParams, sizeof(CalibParams));
							dataOutLenght += sizeof(CalibParams);

							saveModule= CALIB_INPUT + per; //This ensure save on EEPROM
							validAction = true;
						}
						break;

					case ACT_CALIB_READ:
						per = dataIn[5];      //Calib Input Index
						vEEPROMUtils.vReadCalib(per+1);
						dataOut[dataOutLenght++]=ACT_CALIB_READ;
						dataOut[dataOutLenght++]=ucOBSuccess;                                            
						memcpy(dataOut+dataOutLenght,&CalibParams,sizeof(CalibParams));
						dataOutLenght+= sizeof(CalibParams);
						validAction = true;
						saveModule = 0x00;
						break;

					case ACT_SET_BACKOFF:
                        per = dataIn[4];      //Backoff Output Index
                        minutes2Backoff =  dataIn[5];

                        switch(per + 1)
                        {
                            case 1:
                                minutes2BackOffOut1 = minutes2Backoff;
                                break;
                            case 2:
                                 minutes2BackOffOut2 = minutes2Backoff;
                                break;
                            case 3:
                                 minutes2BackOffOut3 = minutes2Backoff;
                                break;
                            case 4:
                                minutes2BackOffOut4 = minutes2Backoff;
								break;
                            case 5:
                                minutes2BackOffOut5 = minutes2Backoff;
								break;
                            case 6:
                                minutes2BackOffOut6 = minutes2Backoff;
								break;
                            case 7:
                                minutes2BackOffOut7 = minutes2Backoff;
								break;
                            case 8:
                                minutes2BackOffOut8 = minutes2Backoff;
								break;
                            case 9:
                                minutes2BackOffOut9 = minutes2Backoff;
								break;
                            case 10:
                                minutes2BackOffOut10 = minutes2Backoff;
								break;
                        }

                        dataOut[dataOutLenght++]=ACT_SET_BACKOFF;
                        dataOut[dataOutLenght++]=ucOBSuccess;
                        dataOut[dataOutLenght++]=ucOBSuccess;
                        dataOut[dataOutLenght++]=minutes2Backoff;

                        Serial.print("NBF: ");
                        Serial.println(minutes2Backoff,DEC);

                        validAction = true;
                        saveModule = OUT1_BACKOFF_TIME+per;
                        break;

                        case ACT_GET_BACKOFF:
                            per = dataIn[4];      //Backoff Output Index

                            minutes2Backoff=0xDE;
                            switch(per+1)
                            {
                                case 1:
                                    minutes2Backoff = minutes2BackOffOut1;
                                    break;
                                case 2:
                                    minutes2Backoff = minutes2BackOffOut2;
                                    break;
                                case 3:
                                    minutes2Backoff = minutes2BackOffOut3;
                                    break;
                                case 4:
                                    minutes2Backoff = minutes2BackOffOut4;
                                    break;
                                case 5:
                                    minutes2Backoff = minutes2BackOffOut5;
									break;
                                case 6:
                                    minutes2Backoff = minutes2BackOffOut6;
									break;
                                case 7:
                                    minutes2Backoff = minutes2BackOffOut7;
									break;
                                case 8:
                                    minutes2Backoff = minutes2BackOffOut8;
									break;
                                case 9:
                                    minutes2Backoff = minutes2BackOffOut9;
									break;
                                case 10:
                                    minutes2Backoff = minutes2BackOffOut10;
									break;
                            }

                            dataOut[dataOutLenght++] = ACT_GET_BACKOFF;
                            dataOut[dataOutLenght++] = ucOBSuccess; 
                            dataOut[dataOutLenght++] = ucOBSuccess;
                            dataOut[dataOutLenght++] = minutes2Backoff;  

                           /* Serial.print("Sent BF: ");
                            Serial.println(minutes2Backoff,DEC);*/

                            validAction = true;
                            saveModule = 0x00;
							break;

					case ACT_AUTHENTICATION: //We are hearing a module trying to authenticate on Master	
						areWeYourDroid = 0x00;
						break;

					case ACT_PING:
						dataOut[dataOutLenght++]=ACT_PING;
						dataOut[dataOutLenght++]=ucOBSuccess;
						validAction = true;
						break;

					default:
						memset(dataOut,0x00,256);
						dataOutLenght = 0x00;
						dataOut[dataOutLenght++]=masterID;
						dataOut[dataOutLenght++]=0xFF;      //Bogus Size
						dataOut[dataOutLenght++]=slaveAddress;
                        dataOut[dataOutLenght++]=dataIn[3]; //Send Also the command
						dataOut[dataOutLenght++]=ucOBError;					
						validAction = true;
						break;
				}

				if(!validAction)
				{
					#if defined(DEBUGOPENBUS) || defined(GENERALDEBUG)
						Serial.println("Er:ucOBResponseTimedOut");
					#endif

					memset(dataOut,0x00,256);
					dataOutLenght = 0x00;
					dataOut[dataOutLenght++]=masterID;
					dataOut[dataOutLenght++]=0xFF;      //Bogus Size
					dataOut[dataOutLenght++]=slaveAddress;
                    dataOut[dataOutLenght++]=dataIn[3]; //Send Also the command
					dataOut[dataOutLenght++]=ucOBError;
					dataOut[1]=dataOutLenght;      //Bogus Size
				}
				else
				{
					dataOut[1]=dataOutLenght;
					crc.CRC16 (dataOut,dataOutLenght,dataOut+dataOutLenght); //Check CRC of data
					memset(dataOut+dataOutLenght+2,0x00,256-dataOutLenght-2);
				}
			}
			/*else
			{
				Serial.println("Not the droid you are looking for!");				
			}*/
		}
		else
		{
             unsigned short yo =0 ;
            for (yo = 0; yo < 256; yo++)
            {
                Serial.print(dataIn[yo],DEC);
                Serial.print(":");
            }
            Serial.println(" ");
            
			memset(dataIn,0x00,256);
			dataOutLenght = 0x00;
			dataOut[dataOutLenght++]=masterID;
			dataOut[dataOutLenght++]=0xFF;      //Bogus Size
			dataOut[dataOutLenght++]=slaveAddress;
            dataOut[dataOutLenght++]=dataIn[3]; //Send Also the command
			dataOut[dataOutLenght++]=ucOBInvalidCRC;
			dataOut[1]=dataOutLenght;
			crc.CRC16 (dataOut,dataOutLenght,dataOut+dataOutLenght); //Check CRC of data
			Serial.println("CRCER");
		}		
		
		if(areWeYourDroid)
		{			
            comsOk = comPort.write_Frame(dataOut,channel); //Send Frame       

			if(comsOk && validAction)
			{
				if( saveModule!= 0x00)
				{	
					vEEPROMUtils.saveModuleConfig(saveModule);
					saveModule = 0x00;				
				}
				return true;
			}
			else
			{
				//Serial.println(F("SF"));
				return false;
			}
		}
		else
		{
			//Serial.println("Not your droid you are looking for again!");
			return false;
		}
	}
	else
	{
		//Serial.println(F("Error Master Response"));
		//comPort.printDetails();
		return false;
	}
}

unsigned char OpenBus::usOpenBusCreateAuthFrame(unsigned char * dataOut,RF24 comPort, unsigned char channel)
{
	//unsigned char dataInLenght;
	unsigned char dataOutLenght;
	bool comsOk = false;
	CRC crc;
    //unsigned char crc16 [2];
	
	//Authentication Frame Description - 14 bytes total
	//[MasterID]:[DataSize]:[SlaveAdress]:[ACTION]:[ACTION_STATE]:[MODULE_TYPE]
	//([Input Type]:[Input ID]:[Input GrowRoomID]:[Input GrowID])* nInputs 
	//[0xFF] Inputs/Outputs Separator, used FF cause for now I think that in data from sensors FF will never appear
	//([Output Type]:[Output ID]:[Output GrowRoomID]:[Output GrowID])* nOutputs 
	//[0xFF] Outputs Ended, used FF cause for now I think that in data from sensors FF will never appear
	//[SerialNumber_0]:[SerialNumber_1]:[SerialNumber_2]:[SerialNumber_3]:[SerialNumber_4]:[SerialNumber_5]:[SerialNumber_5]:[SerialNumber_7]
	//[CRC16_HNb]:[CRC16_LNb]
	memset(dataOut,0x00,256);				//Clear Buffer
	dataOutLenght=0;
	dataOut[dataOutLenght++]= masterID;		//Our current Slave Address
	dataOut[dataOutLenght++]= 0xFF;			//Bogus size of dataOut cause we still don't know it				
	dataOut[dataOutLenght++]= slaveAddress;
	dataOut[dataOutLenght++]= ACT_AUTHENTICATION;
	dataOut[dataOutLenght++]= botID;
	dataOut[dataOutLenght++]= botType;
	
	for(oB=0;oB<MAX_INPUTS;oB++)			//Dump all inputs
	{
		if(inputs[oB].type!=0x00)
		{					
			dataOut[dataOutLenght++]= inputs[oB].type;		//Create Temperature Sensor
			dataOut[dataOutLenght++]= oB;					//Temperature Sensor ID
			dataOut[dataOutLenght++]= 0;					//Assign GrowRoom to sensor
			dataOut[dataOutLenght++]= 0;					//Assign Grow to sensor	
		}
	}
	
	dataOut[dataOutLenght++] = 0xFF;		//Inputs/Outputs Separator
	
	for(oB=0;oB<MAX_OUTPUTS;oB++)			//Dump all outputs
	{
		if(outputs[oB].type!=0x00)
		{					
			dataOut[dataOutLenght++]= outputs[oB].type;		//Create Temperature Sensor
			dataOut[dataOutLenght++]= oB;					//Temperature Sensor ID
			dataOut[dataOutLenght++]= 0;					//Assign GrowRoom to sensor
			dataOut[dataOutLenght++]= 0;					//Assign Grow to sensor				
		}
	}
	dataOut[dataOutLenght++] = 0xFF;		//Outputs Ended

	//Take the Chance to see if our firmware was changed 
	if(fwVersion[0] != FW_1 || fwVersion[1] != FW_2 || fwVersion[2] != FW_3 || fwVersion[3] != FW_4)
	{
		fwVersion[0] = FW_1;
		fwVersion[1] = FW_2;
		fwVersion[2] = FW_3;
		fwVersion[3] = FW_4;
		eeprom_write_block(fwVersion, (void *)(FW_EEPROM_START), sizeof(fwVersion));		
		Serial.print("RST FW");			
	}

	dataOut[dataOutLenght++] = fwVersion[0];
	dataOut[dataOutLenght++] = fwVersion[1];
	dataOut[dataOutLenght++] = fwVersion[2];
	dataOut[dataOutLenght++] = fwVersion[3];

	//Send also serial number for all of them
	memcpy(dataOut+dataOutLenght,serialNumber,8);
	dataOutLenght=dataOutLenght+8;
	memcpy(dataOut+dataOutLenght,name,15);
	dataOutLenght=dataOutLenght+15;
	dataOut[1] = dataOutLenght;
	crc.CRC16 (dataOut,dataOutLenght,dataOut+dataOutLenght);		//Make frame with slaveAddress, sensors and outputs config, serialNumber and CRC
	dataOutLenght = dataOutLenght + 2;								//

	comsOk = comPort.write_Frame(dataOut,channel); //Send Frame

	if(comsOk)
	{
		//Serial.println(F("SK"));
		return true;
	}
	else
	{
		//Serial.println(F("SF"));
		return false;
	}
}

/****************************************************************************/