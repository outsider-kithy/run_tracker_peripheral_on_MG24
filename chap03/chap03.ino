#include "getAcceralate.h"
#include "getHeartRate.h"
#include "setDisplay.h"

void setup(){
  Serial.begin(9600);
  setAcceralate();
  setHeartRate();
  setDisplay();
}

void loop(){
  getAcceralate();
  getHeartRate();
}