#pragma once
#include <TinyGPS++.h>

struct Point {
  double lat;
  double lng;
};

extern Point path[];
extern int pathCount;

// 総移動距離（m）
extern double totalDistance; 

extern void getGps();