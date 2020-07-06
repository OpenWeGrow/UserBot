/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "GroBot_Variables.h"
#include "SensorsTask.h"
#include <Arduino.h>
#include "ComsTask.h"
#include <DHT.h>

#define MAX_USER_OUTPUTS 8

DHT dht(A2,DHT22);

sensorsMachineState snsState;
unsigned char stMachineExternalControl = 0x00;

#define ArduinoVRef  5

int time= 0;
unsigned char outputIndex2Act = 0;

//DHT Error Management
unsigned char dhtErrorCount = 0;
unsigned char dhtTempInternalCount = 0;
unsigned char dhtHumiInternalCount = 0;
float temp;

SensorsTask::SensorsTask(void)
{
	snsState = INIT_SENSORS;
}

void SensorsTask::GoSensorsTask(void)
{
    //You can create has many states as you would like, just look at SensorsTask.h and change the enum states
    //Make sure you also implement the state on this file
    //This example has a state to initialize, and other to constantly check if any output changed
	
	//snsState = ACT_ON_IOS;
	
	
	switch(snsState)
    {
		case INIT_SENSORS:
            //Use this state in the machine to initialize any sensor you may need
                    
            

            //Initialize the timer to verify backoff/cooldown feature
            ticksOut1 = millis();
            ticksOut2 = millis();
            ticksOut3 = millis();
            ticksOut4 = millis();
            ticksOut5 = millis();
            ticksOut6 = millis();
            ticksOut7 = millis();
            ticksOut8 = millis();
            dht.begin();
			snsState = CHECK_DHT;
			break;
	
        case CHECK_DHT:
            //noInterrupts();
            handleDHT22Temp();
           // interrupts();
            /*Serial.print(F("Temp: ")); 
            Serial.print(inputs[INPUT_INDEX0].value); 
            Serial.print(F("C "));  
            Serial.print(F("Humi:")); 
            Serial.print(inputs[INPUT_INDEX1].value); 
            Serial.println(F("%")); */
            
            snsState = ACT_ON_IOS;
            break;
            
		case ACT_ON_IOS:		
            //Act on Output 1
            for(outputIndex2Act =0 ; outputIndex2Act<MAX_USER_OUTPUTS; outputIndex2Act++)
                actOnOutput(outputIndex2Act); 
            
			snsState = CHECK_DHT;
			break;      
	}
}

void SensorsTask::actOnOutput(unsigned char indexOutput)
{
    unsigned long outputTicks;
    unsigned char outputMinutes2BackOff;
    
    //Search the correct timeout for the cooldown feature
    switch(indexOutput)
    {
        case OUTPUT_INDEX0:
            outputTicks = ticksOut1;
            outputMinutes2BackOff = minutes2BackOffOut1;
        break;
        
        case OUTPUT_INDEX1:
            outputTicks = ticksOut2;
            outputMinutes2BackOff = minutes2BackOffOut2;
        break;
        
        case OUTPUT_INDEX2:
            outputTicks = ticksOut3;
            outputMinutes2BackOff = minutes2BackOffOut3;
        break;
        
        case OUTPUT_INDEX3:
            outputTicks = ticksOut4;
            outputMinutes2BackOff = minutes2BackOffOut4;
        break;
        
        case OUTPUT_INDEX4:
            outputTicks = ticksOut5;
            outputMinutes2BackOff = minutes2BackOffOut5;
        break;
        
        case OUTPUT_INDEX5:
            outputTicks = ticksOut6;
            outputMinutes2BackOff = minutes2BackOffOut6;
        break;
        
        case OUTPUT_INDEX6:
            outputTicks = ticksOut7;
            outputMinutes2BackOff = minutes2BackOffOut7;
        break;
        
        case OUTPUT_INDEX7:
            outputTicks = ticksOut8;
            outputMinutes2BackOff = minutes2BackOffOut8;
        break;
        
        case OUTPUT_INDEX8:
            outputTicks = ticksOut9;
            outputMinutes2BackOff = minutes2BackOffOut9;
        break;
        
        case OUTPUT_INDEX9:
            outputTicks = ticksOut10;
            outputMinutes2BackOff = minutes2BackOffOut10;
        break;
            
    }
    

    if(outputs[indexOutput].value>0)
    {
        //If not in cooldown turn on the output
        if((millis() - outputTicks) > (MILLIS_PER_MINUTE * outputMinutes2BackOff) )
        {
            if(outputs[indexOutput].speed == 0)
                digitalWrite(outputs[indexOutput].arduinoPin, HIGH);
            else
                analogWrite(outputs[indexOutput].arduinoPin, outputs[indexOutput].speed);  
        } 
    }
    else
    {
        //Shutdown the output, no cooldown verification needed
        digitalWrite(outputs[indexOutput].arduinoPin, LOW);
    }   
    
}

void SensorsTask::handleDHT22Temp()
{
  
 //sensors_event_t event;
 // Serial.println(F("Get Data 1"));
  temp = dht.readTemperature();
 // Serial.println(F("Get Data 2"));
  if (isnan(temp))
  {  
      Serial.println(F("Error temp"));
      dhtErrorCount++;
  }
  else 
  {      
    if(temp > 80 || temp < -40) //Temperature value has to be between -40 and 80
    {
      //Serial.println(F("Temperature out of bounds!"));
      dhtErrorCount++; 
    }
    else
    {   
        if(validateNewValue(inputs[INPUT_INDEX0].value,temp,10)||dhtTempInternalCount >= 5) // and connot have a deviation higher than 20
        {
           /* if(!dhtTempInternalCount>=5)
            {
               //Serial.println(F("Variation Ok")); 
            }*/
            dhtTempInternalCount=0;
            inputs[INPUT_INDEX0].value += (double)temp;
            inputs[INPUT_INDEX0].value = inputs[INPUT_INDEX0].value/2;
            
           
        }
        else
        {
          dhtTempInternalCount++;
          //Serial.println(F("Temperature variation too high!"));
          dhtErrorCount++;
        }
        
    }
  }
  
  // Get humidity event and print its value.
  temp = dht.readHumidity();
  
  if (isnan(temp))
  {
      Serial.println(F("Error humi"));
      dhtErrorCount++;
  }
  else
  {
    if(temp > 100 || temp < 0 ) //Humidity value has to be between 0 and 100
    {
      //Serial.println(F("Humidity out of bounds!"));
      dhtErrorCount++; 
    }
    else {
   
        if(validateNewValue(inputs[INPUT_INDEX1].value,temp,20)||dhtHumiInternalCount >= 5) // and connot have a deviation higher than 20
        {
           /* if(dhtHumiInternalCount>=5)
            {
                //Serial.println(F("Broke via counter!")); 
            }
            else
            {
                Serial.println(F("Variation Ok")); 
            }*/
            
            dhtHumiInternalCount = 0;        
            inputs[INPUT_INDEX1].value += (double)temp;
            inputs[INPUT_INDEX1].value = inputs[INPUT_INDEX1].value/2;
            
        }
        else
        {
          dhtHumiInternalCount++;
          //Serial.println(F("Humidty variation too high!"));
          dhtErrorCount++;
        }
    }
  }
  //Serial.println(F("Get Data 3"));
  //dhtErrorCount++;
  
  
}

bool SensorsTask::validateNewValue(double latestValidRead, double newValue, double validationFactor)
{
    if (newValue >= (latestValidRead + validationFactor)) { return false; }
    if (newValue <= (latestValidRead - validationFactor)) { return false; }
    return true;
}

void SensorsTask::printOutputState(unsigned char indexOutput)
{
    
    Serial.print(F("Output "));
    Serial.print(indexOutput+1,DEC);
    Serial.print(F("- "));
    
        if(reversedOutputs)
        {
            if(outputs[indexOutput].value == 0)
            {
                Serial.print(F("ON Sp-"));
            }
            else
            {
                Serial.print(F("OFF Sp-"));
            }
        }
        else
        {
            if(outputs[indexOutput].value == 0)
            {
                Serial.print(F("OFF Sp-"));
            }
            else
            {
                Serial.print(F("ON Sp-"));
            }		
        }
    
    Serial.print(outputs[indexOutput].speed,DEC);
    Serial.print(" CD: ");
    
    switch(indexOutput)
    {
        case OUTPUT_INDEX0:
            Serial.print(minutes2BackOffOut1);
            
            break;
            
        case OUTPUT_INDEX1:
            Serial.print(minutes2BackOffOut2);
          //  Serial.println(" minutes");
            break;            
            
        case OUTPUT_INDEX2:
            Serial.print(minutes2BackOffOut3);
            //Serial.println(" minutes");
            break;
            
            
        case OUTPUT_INDEX3:
            Serial.print(minutes2BackOffOut4);
           // Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX4:
            Serial.print(minutes2BackOffOut5);
            //Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX5:
            Serial.print(minutes2BackOffOut6);
            //Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX6:
            Serial.print(minutes2BackOffOut7);
            //Serial.println(" minutes");
            break;
            
        case OUTPUT_INDEX7:
            Serial.print(minutes2BackOffOut8);
            //Serial.println(" minutes");
            break;
    }
    Serial.println(" m");
}



/****************************************************************************/