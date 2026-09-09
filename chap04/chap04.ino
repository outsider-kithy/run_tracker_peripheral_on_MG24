#include "getAcceralate.h"
#include "getHeartRate.h"
#include "setDisplay.h"
#include "displayClock.h"

void setup(){
  Serial.begin(9600);
  setAcceralate();
  setHeartRate();
  setDisplay();
  getTime();
}

void loop(){
  getAcceralate();
  getHeartRate();
  showTime();
}