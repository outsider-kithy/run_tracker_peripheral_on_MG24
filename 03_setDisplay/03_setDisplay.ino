#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// TFTピン設定
#define TFT_CS    D2
#define TFT_DC    D1
#define TFT_RST   D0

// ST7789初期化
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // put your setup code here, to run once:
  // ディスプレイ初期化
  tft.init(240, 240);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("Get Ready.");
}

void loop() {
  // put your main code here, to run repeatedly:

}
