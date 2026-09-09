# run_tracker_peripheral_on_MG24

Seeed Studio XIAO MG24 Senceで作るランニングトラッカー

## BOM(Bill of Material)
- Seeed Studio XIAO MG24 Sence 1個
- TFT LCDモジュール（1.54インチ 240×240）１個
- MAX30102(心拍数センサー) 1個
- ATGM336H(GPS受信モジュール) 1個
- SS-12D00-G5(基板用スライドスイッチ) １個

※ブレッドボード上でのプロトタイプ作成には、以下も必要
- ブレッドボード
- ジャンパ線　10cm～20cm オスｰオス　20本程度
- ピンヘッダ(2.54mmピッチ)

## 回路

|	Seeed Studio XIAO	|	MAX30102	|
|-----------------------|---------------|
|	3V3	|	VIN	|
|	GND	|	GND	|
|	D5	|	SCL	|
|	D4	|	SDA	|


|	Seeed Studio XIAO	|	TFT LCD	|
|-----------------------|-----------|
|	3V3	|	VCC	|
|	GND	|	GND	|
|	D0	|	RES	|
|	D1	|	DC	|
|	D2	|	CS	|
|   D3  |   BL  |
|	D8	|	SCL	|
|	D10	|	SDA	|


|	Seeed Studio XIAO	|	ATGM336H	|
|-----------------------|---------------|
|	3V3	|	VCC	|
|	GND	|	GND	|
|	D7	|	TX	|
|	D9	|	RX	|


|	Seeed Studio XIAO	|	SS-12D00-G5	|
|-----------------------|---------------|
|	D6	|	中央	|
|	GND	|	片端	|
