#include "getAcceralate.h"
#include "getHeartRate.h"

void setup(){
  Serial.begin(9600);
  setAcceralate();
  setHeartRate();
}

void loop(){
  getAcceralate();
  getHeartRate();
}