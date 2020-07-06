/*
 Copyright (C) 2019 Open Grow - GroLab, Author: JMelo <joao.melo@opengrow.pt>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __SENSORSTASK_H__
#define __SENSORSTASK_H__


// ENUM used in the switch cases from SensorsTask.cpp (UserBot folder)
enum sensorsMachineState {
  INIT_SENSORS = 0,
  CHECK_DHT,
  ACT_ON_IOS
};
extern sensorsMachineState snsState;

class SensorsTask
{
private:
	

protected:

public:
	SensorsTask(void);
	void GoSensorsTask(void);
    void handleDHT22Temp();
	bool validateNewValue(double latestValidRead, double newValue, double validationFactor);
    void printOutputState(unsigned char indexOutput);
	void actOnOutput(unsigned char indexOutput);
};

#endif // 