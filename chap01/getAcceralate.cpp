#include "getAcceralate.h"

// IMUの設定
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
float aX, aY, aZ;

// 加速度のしきい値（この値を超えたら1歩とカウント）
const float STEP_THRESHOLD = 1.2;  
// 1歩あたりの距離（m）※目安：歩幅約0.7m
const float STEP_LENGTH = 0.7;     

// 状態変数
int steps = 0;
bool stepActive = false;

void setAcceralate(){
  Serial.begin(9600);
   // IMUの初期化
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("IMU is available.");
  }
}

void getAcceralate(){
  aX = myIMU.readFloatAccelX();
  aY = myIMU.readFloatAccelY();
  aZ = myIMU.readFloatAccelZ();

  float magnitude = sqrt(aX*aX + aY*aY + aZ*aZ);

  if(magnitude > STEP_THRESHOLD && !stepActive){
    stepActive = true;
    steps++;
  }

  if(magnitude < 0.9){
    stepActive = false;
  }
  Serial.printf("aX=%.2f,",aX);
  Serial.printf("aY=%.2f,",aY);
  Serial.printf("aZ=%.2f\n",aZ);
  // Serial.print("Steps:");
  // Serial.println(steps);
}