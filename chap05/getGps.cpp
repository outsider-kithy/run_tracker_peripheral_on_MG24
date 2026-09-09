#include "getGps.h"

TinyGPSPlus gps;

#define MAX_PATH 500
Point path[MAX_PATH];

int pathCount = 0;

// 総移動距離（m）
double totalDistance = 0.0; 

unsigned long lastSaveTime = 0;
const unsigned long SAVE_INTERVAL = 20000; // 20秒

void getGps(){
  // 緯度経度を取得
  while (Serial1.available()) {
      char c = Serial1.read();
      gps.encode(c); // TinyGPS++へ渡す
    }

     // 20秒経過していなければ何もしない
    if (millis() - lastSaveTime < SAVE_INTERVAL) {
      return;
    }

    lastSaveTime = millis();

    double lat = gps.location.lat();
    double lng = gps.location.lng();

    Serial.print("lat:");
    Serial.println(lat);
    Serial.print("lng:");
    Serial.println(lng);
    
    // Pathにgps.location.lat()とgps.location.lng()を追加
   if(pathCount < MAX_PATH){
     // 総移動距離を追加
      if (pathCount > 0) {
        double prevLat = path[pathCount - 1].lat;
        double prevLng = path[pathCount - 1].lng;
        double dist = TinyGPSPlus::distanceBetween(prevLat, prevLng, lat, lng);
        totalDistance += dist;
      }
      path[pathCount].lat = lat;
      path[pathCount].lng = lng;
      pathCount++;
    }
}
