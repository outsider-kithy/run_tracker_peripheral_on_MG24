#include <Arduino.h>

extern "C" {
#include "nvm3.h"
#include "nvm3_default.h"
}

#define MAX_JSON_SIZE  1024

// ======================================================
// NVM3 Object Key
// ======================================================
#define JSON_OBJECT_KEY 0x40001

// ======================================================
// ダミーデータ
// ======================================================
struct Point {
    double lat;
    double lng;
};
Point path[] = {
      {35.677374, 139.751395},
      {35.683944, 139.744177},
      {35.694305, 139.746564},
      {35.695394, 139.751392},
      {35.692859, 139.755812}
    };
  int pathCount = sizeof(path) / sizeof(path[0]);
  double totalDistance = 100.0;
  float averageBPM = 70.0;
  float averageTemperature = 28.0;
  int steps = 5620;
  uint32_t elapsedSeconds = 1230;
  String startDate = "2026-08-29 10:00:00";
  String endDate = "2026-08-29 10:20:30";

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
void deleteJsonFromNvm3() {

  Ecode_t err = nvm3_deleteObject(
    nvm3_defaultHandle,
    JSON_OBJECT_KEY
  );

  if (err == ECODE_NVM3_OK) {
    Serial.println("JSON deleted");
    // 今あるJSONを表示
    loadJsonFromNvm3();
  } else {
    Serial.print("Delete failed: ");
    Serial.println(err);
  }
}


void setup() {
  Serial.begin(9600);
  // NVM3を初期化
  nvm3_setup();
  // JSONを作成
  String json = createJson();
  // JSONを保存
  saveJsonToNvm3(json);
  // 今あるJSONを表示
  loadJsonFromNvm3();
}

void loop() {
   if (Serial.available()) {
    char c = Serial.read();
    if (c == 'd') {
      deleteJsonFromNvm3();
    }
  }
}
