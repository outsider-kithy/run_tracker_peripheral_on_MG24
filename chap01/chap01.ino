#include "getAcceralate.h"

void setup(){
  Serial.begin(9600);
  setAcceralate();
}

void loop(){
  getAcceralate();
}