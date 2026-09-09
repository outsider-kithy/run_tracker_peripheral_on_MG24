#pragma once
#include <Wire.h>
#include <cstdint>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;

extern void getTime();
extern void showTime();