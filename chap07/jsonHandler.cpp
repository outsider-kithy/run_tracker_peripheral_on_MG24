#include "jsonHandler.h"
#include "getAcceralate.h"
#include "getHeartRate.h"
#include "getGps.h"
#include "displayClock.h"

extern "C" {
#include "nvm3.h"
#include "nvm3_default.h"
}

#define MAX_JSON_SIZE  1024

// ======================================================
// NVM3 Object Key
// ======================================================
#define JSON_OBJECT_KEY 0x40001

// NVM3を初期化
void nvm3_setup(){
  Ecode_t err = nvm3_initDefault();
  Serial.print("nvm3_initDefault result: ");
  Serial.println(err);
  if (err == ECODE_NVM3_OK) {
    Serial.println("NVM3 ready");
  }
}

// ======================================================
// JSON生成
// ======================================================
String createJson(){
  String json = "{";

  // points
  json += "\"points\":[";

  for(int i = 0; i < pathCount; i++){
    json += "{";
    json += "\"lat\":";
    json += String(path[i].lat, 6);
    json += ",";
    json += "\"lng\":";
    json += String(path[i].lng, 6);
    json += "}";

    // 最後以外はカンマ
    if(i < pathCount - 1){
      json += ",";
    }
  }
  json += "],";

  // distance
  json += "\"distance\":";
  json += String(totalDistance, 2);
  json += ",";

  // //Avg BPM
  json += "\"Avg BPM\":";
  json += averageBPM;
  json += ",";

  // // temp
  json += "\"Temp\":";
  json += averageTemperature;
  json += ",";

  // // steps
  json += "\"steps\":";
  json += String(steps);
  json += ",";
  
  // // elapsedSeconds
  json += "\"elapsedSeconds\":";
  json += String(elapsedSeconds);
  json += ",";

  // startDate
  json += "\"startDate\":\"";
  json += startDate;
  json += "\",";

  // endDate
  json += "\"endDate\":\"";
  json += endDate;
  json += "\"";

  json += "}";

  Serial.print("Created JSON:");
  Serial.println(json);

  return json;
}

// ======================================================
// JSON保存
// ======================================================
bool saveJsonToNvm3(
    const String& json
)
{
Serial.print("JSON length=");
Serial.println(json.length());

Serial.print("KEY=");
Serial.println(JSON_OBJECT_KEY, HEX);

Serial.print("HANDLE=");
Serial.println(
  (uint32_t)nvm3_defaultHandle
);

uint32_t length =
      json.length() + 1;
  char buffer[length];
  memcpy(
    buffer,
    json.c_str(),
    length
  );

  Ecode_t err =
    nvm3_writeData(
      nvm3_defaultHandle,
      JSON_OBJECT_KEY,
      buffer,
      length
    );

  Serial.print("write result:");
  Serial.println(err);

  Serial.print("Saved JSON:");
  Serial.println(json);

  delay(1000);

  return err == ECODE_NVM3_OK;
}

// ======================================================
// JSON読み込み
// ======================================================
String loadJsonFromNvm3() {

  uint32_t objectSize = 0;
  uint32_t objectType = 0;

  // JSONオブジェクトが存在するか確認
  Ecode_t infoErr = nvm3_getObjectInfo(
    nvm3_defaultHandle,
    JSON_OBJECT_KEY,
    &objectType,
    nullptr
  );

  // オブジェクトが存在しない
  if (infoErr != ECODE_NVM3_OK) {
    Serial.println("No JSON");
    return "";
  }

  // サイズチェック
  if (objectSize == 0) {
    Serial.println("No JSON");
    return "";
  }

  if (objectSize > MAX_JSON_SIZE) {
    Serial.print("JSON too large: ");
    Serial.println(objectSize);
    return "";
  }

  // +1 は終端文字 '\0' 用
  char jsonBuffer[MAX_JSON_SIZE + 1];

  memset(jsonBuffer, 0, sizeof(jsonBuffer));

  // NVM3から読み出す
  Ecode_t err = nvm3_readData(
    nvm3_defaultHandle,
    JSON_OBJECT_KEY,
    jsonBuffer,
    objectSize
  );

  if (err != ECODE_NVM3_OK) {
    Serial.print("Read failed: ");
    Serial.println(err);
    return "";
  }

  // 念のため終端文字を設定
  jsonBuffer[objectSize] = '\0';

  String json = String(jsonBuffer);

  Serial.print("JSON size: ");
  Serial.println(objectSize);

  Serial.println("Loaded JSON:");
  Serial.println(json);

  return json;
}

// ======================================================
// JSON削除
// ======================================================
bool deleteJsonFromNvm3() {
  sl_status_t status = nvm3_deleteObject(
    nvm3_defaultHandle,
    JSON_OBJECT_KEY
  );

  if (status == ECODE_NVM3_OK) {
    Serial.println("JSON deleted");
    return true;
  }

  Serial.print("Delete error: ");
  Serial.println(status);

  return false;
}


