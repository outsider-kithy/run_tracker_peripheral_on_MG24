#pragma once
#include <Arduino.h>

extern void nvm3_setup();
extern String createJson();
extern bool saveJsonToNvm3(const String& json);
extern String loadJsonFromNvm3();
extern bool deleteJsonFromNvm3();