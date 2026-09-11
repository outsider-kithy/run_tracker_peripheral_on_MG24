#include "displayClock.h"

unsigned long syncMillis = 0;
uint32_t baseEpoch = 1788220800;
bool clock_initialized = false;

String startDate = "";
String endDate = "";
uint32_t startEpoch = 0;
uint32_t endEpoch = 0;
uint32_t elapsedSeconds = 0;

void getTime() {
  syncMillis = millis();
  clock_initialized = true;
  static unsigned long prev = 0;
  if (millis() - prev >= 1000) {
    prev = millis();
    showTime();
  }
}

DateTime getNow() {
  if (!clock_initialized) {
    return DateTime(2000,1,1,0,0,0);
  }
  uint32_t currentEpoch =
    baseEpoch +
    ((millis() - syncMillis) / 1000);
  return DateTime(currentEpoch);
}

void showTime() {
  DateTime now = getNow();
  // JSTへ変換
  uint32_t jstEpoch = now.unixtime() + (9 * 60 * 60);
  DateTime jst(jstEpoch);
  // 前回値保存
  static int oldYear = -1;
  static int oldMonth = -1;
  static int oldDay = -1;
  static int oldHour = -1;
  static int oldMinute = -1;
  static int oldSecond = -1;

  /*
   * 日付更新
   */
  if (
    jst.year() != oldYear ||
    jst.month() != oldMonth ||
    jst.day() != oldDay
  ) {

    // 古い日付を黒で消す
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(4);
    tft.setCursor(10,20);
    tft.print(oldYear);
    tft.print("/");
    tft.print(oldMonth);
    tft.print("/");
    tft.print(oldDay);

    // 新しい日付
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(10,20);
    tft.print(jst.year());
    tft.print("/");
    tft.print(jst.month());
    tft.print("/");
    tft.print(jst.day());

    oldYear = jst.year();
    oldMonth = jst.month();
    oldDay = jst.day();
  }

  /*
   * 時刻更新
   */
  if (
    jst.hour() != oldHour ||
    jst.minute() != oldMinute
  ) {

    // 古い HH:MM を消す
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(4);
    tft.setCursor(10,60);

    if(oldHour >= 0){
      tft.print(oldHour);
      tft.print(":");

      if(oldMinute < 10)
        tft.print("0");
      tft.print(oldMinute);
      tft.print(":");
    }

    // 新しい HH:MM
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(10,60);
    tft.print(jst.hour());
    tft.print(":");

    if(jst.minute() < 10)
      tft.print("0");
    tft.print(jst.minute());
    tft.print(":");
    oldHour = jst.hour();
    oldMinute = jst.minute();

  }

  /*
   * 秒更新
   */
  if(
    jst.second() != oldSecond
  ){

    // 古い秒を消す
    tft.setTextColor(ST77XX_BLACK);
    tft.setCursor(150,60);

    if(oldSecond >= 0){
      if(oldSecond < 10)
        tft.print("0");
      tft.print(oldSecond);
    }

    // 新しい秒
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(150,60);
    if(jst.second() < 10)
      tft.print("0");
    tft.print(jst.second());
    oldSecond = jst.second();
  }
}

//現在時刻を取得
String getDateTimeString(){

  DateTime now = getNow();

  // JSTへ変換
  uint32_t jstEpoch = now.unixtime() + (9 * 60 * 60);
  DateTime jst(jstEpoch);
  char buffer[20];
  snprintf(
    buffer,
    sizeof(buffer),
    "%04d-%02d-%02d %02d:%02d:%02d",
    jst.year(),
    jst.month(),
    jst.day(),
    jst.hour(),
    jst.minute(),
    jst.second()
  );
  return String(buffer);
}


