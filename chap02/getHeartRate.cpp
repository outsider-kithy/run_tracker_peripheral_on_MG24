#include "getHeartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4;  //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;  //Time at which the last beat occurred

float beatsPerMinute;  //Current BPM value
int beatAvg;           //Average BPM value

//各種平均値
float totalBPM = 0;
int bpmCount = 0;
float averageBPM = 0;

float totalTemperature = 0;
int temperatureCount = 0;
float averageTemperature = 0;

// SpO2計算用
#define BUFFER_SIZE 100
uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];
int bufferIndex = 0;
float spo2 = 0;

float calculateSpO2() {
  double redMean = 0;
  double irMean = 0;

  // DC成分（平均値）を計算
  for (int i = 0; i < BUFFER_SIZE; i++) {
    redMean += redBuffer[i];
    irMean += irBuffer[i];
  }

  redMean /= BUFFER_SIZE;
  irMean /= BUFFER_SIZE;

  // AC成分（変動成分）のRMSを計算
  double redAC = 0;
  double irAC = 0;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    double redDiff = redBuffer[i] - redMean;
    double irDiff = irBuffer[i] - irMean;
    redAC += redDiff * redDiff;
    irAC += irDiff * irDiff;
  }

  redAC = sqrt(redAC / BUFFER_SIZE);
  irAC = sqrt(irAC / BUFFER_SIZE);


  if (redMean == 0 || irMean == 0 || redAC == 0 || irAC == 0) {
    return 0;
  }

  /* R =
   * (ACred / DCred)
   * ----------------
   * (ACir / DCir)
   */

  double R = (redAC / redMean) / (irAC / irMean);

  //簡易的なSpO2近似式
  float calculatedSpO2 = 110.0 - (25.0 * R);

  // 異常値を除外
  if (calculatedSpO2 > 100) {
    calculatedSpO2 = 100;
  }
  if (calculatedSpO2 < 0) {
    calculatedSpO2 = 0;
  }
  return calculatedSpO2;
}

void setHeartRate() {
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))  //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1) ; //Infinite loop to stop the program
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
}

void getHeartRate() {
  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  if (checkForBeat(irValue) == true) {
    //Calculate beatsPerMinute
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {  //Check if the BPM value is within a valid range
      rates[rateSpot++] = (byte)beatsPerMinute;         //Store this reading in the array
      rateSpot %= RATE_SIZE;                            //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // SpO2用データ保存
  irBuffer[bufferIndex] = irValue;
  redBuffer[bufferIndex] = redValue;
  bufferIndex++;

  // 100サンプル取得したらSpO2計算
  if (bufferIndex >= BUFFER_SIZE) {

    spo2 = calculateSpO2();

    Serial.print("BPM=");
    Serial.print(beatAvg);

    Serial.print(" SpO2=");
    Serial.print(spo2, 1);
    Serial.println("%");
    bufferIndex = 0;
  }

  //Print the IR value, current BPM value, and average BPM value to the serial monitor
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
}


