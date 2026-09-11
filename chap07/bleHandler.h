#pragma once
#include "jsonHandler.h"

extern volatile bool rtc_update_requested;
extern uint32_t receivedEpoch;

extern volatile bool json_requested;

extern volatile bool delete_requested;

extern void syncRtcNotification();
extern void sendJsonToCentral(String json);
extern void deleteAllJsonFiles();