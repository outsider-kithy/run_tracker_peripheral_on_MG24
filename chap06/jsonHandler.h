#pragma once
#include <Arduino.h>
#include "getAcceralate.h"
#include "getHeartRate.h"
#include "getGps.h"
#include "displayClock.h"

extern void nvm3_setup();
extern String createJson();
extern bool saveJsonToNvm3(const String& json);
extern String loadJsonFromNvm3();
extern bool deleteJsonFromNvm3();