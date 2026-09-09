#include "getAcceralate.h"
#include "getHeartRate.h"
#include "setDisplay.h"
#include "displayClock.h"
#include "getGps.h"

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  setAcceralate();
  setHeartRate();
  setDisplay();
  getTime();
}

void loop(){
  getAcceralate();
  getHeartRate();
  showTime();
  getGps();
}