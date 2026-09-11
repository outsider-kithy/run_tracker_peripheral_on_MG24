#pragma once
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

// 各種平均値
extern float totalBPM;
extern int bpmCount;
extern float averageBPM;

extern float totalTemperature;
extern int temperatureCount;
extern float averageTemperature;

extern void setHeartRate();
extern void getHeartRate();