#include "getAcceralate.h"
#include "getHeartRate.h"
#include "setDisplay.h"
#include "displayClock.h"
#include "getGps.h"
#include "jsonHandler.h"
#include "bleHandler.h"
#include "sl_bluetooth.h"

#define SWITCH_PIN 6

bool measuring = false;
bool sensorInitialized = false;

bool jsonCreated = false;
String json;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);

  // スライドスイッチの値を読み取る
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  //RTCの同期を要求
  syncRtcNotification();
  delay(1000);

  //今あるJSONを表示
  //loadJsonFromNvm3();

  setAcceralate();
  setHeartRate();
  setDisplay();
  delay(1000);
}

void loop(){

  if(rtc_update_requested){
      getTime();
      rtc_update_requested=false;
  }

  bool switchState = digitalRead(SWITCH_PIN);
   if(switchState == LOW  && !measuring){   // スイッチON
      Serial.print("Measurement Start, ");
      Serial.println(switchState);
      measuring = true;

      // 各種平均値リセット
      totalBPM = 0;
      bpmCount = 0;
      totalTemperature = 0;
      temperatureCount = 0;
      
      // センサー初期化
      setHeartRate();
      setAcceralate();
      sensorInitialized = true;

      //開始時刻を記録
      startDate = getDateTimeString();
      Serial.print("Start Date: ");
      Serial.println(startDate);
      startEpoch = getNow().unixtime();

      //ディスプレイにStartと表示
      tft.fillRect(0, 180, 240, 240, ST77XX_BLACK);
      tft.setTextColor(ST77XX_CYAN);
      tft.setCursor(10, 180);
      tft.println("Start!");
      delay(1000);

  }else if(switchState == HIGH  && measuring){  // スイッチOFF
      Serial.print("Measurement Stop, ");
      Serial.println(switchState);
      measuring = false;
      sensorInitialized = false;

      //終了時刻を記録
      endDate = getDateTimeString();
      Serial.print("End Date: ");
      Serial.println(endDate);

      //経過時間を計算
      endEpoch = getNow().unixtime();
      elapsedSeconds = endEpoch - startEpoch;
      Serial.print("Elapsed Seconds: ");
      Serial.println(elapsedSeconds);

      //各種平均値を計算
      if(bpmCount > 0){
        averageBPM = totalBPM / bpmCount;
      }
      Serial.print("Average BPM:");
      Serial.println(averageBPM);

      if(temperatureCount > 0){
        averageTemperature = totalTemperature / temperatureCount;
      }
      Serial.print("Average Temp:");
      Serial.println(averageTemperature);

      //ディスプレイに表示
      tft.fillRect(0, 180, 240, 240, ST77XX_BLACK);
      tft.setTextColor(ST77XX_CYAN);
      tft.setCursor(10, 180);
      tft.println("Stop.");
      delay(1000);

      // JSON生成許可
      jsonCreated = false;
  }

  // スイッチONの間だけ計測
  if(measuring){
    getHeartRate();
    getAcceralate();
  }
  
  // 時計とGPSは常時稼働
  getTime();
  getGps();

  //JSONを作成し、保存
  if(!measuring && elapsedSeconds > 0 && !jsonCreated){
    // JSONを作成
    Serial.println("Create JSON");
    json = createJson();
    jsonCreated = true;
    // JSONを保存
    saveJsonToNvm3(json);
  }

  // JSONをセントラルに送信
  if (json_requested) {
      sendJsonToCentral(json);
      delay(1000);
      json_requested = false;
  }

	// // セントラルからDELETEコマンドが送られてきたら
  if(delete_requested){
		deleteAllJsonFiles();
    delete_requested = false;
  }

}