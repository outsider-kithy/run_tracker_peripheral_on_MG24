#pragma once
#include <Wire.h>
#include <cstdint>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;

extern String startDate;
extern String endDate;
extern uint32_t startEpoch;
extern uint32_t endEpoch;
extern uint32_t elapsedSeconds;

extern void getTime();
extern void showTime();
extern DateTime getNow();
extern String getDateTimeString();