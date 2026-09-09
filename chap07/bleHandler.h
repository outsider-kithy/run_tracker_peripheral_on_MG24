#pragma once
#include "jsonHandler.h"

// extern uint16_t gattdb_session_id;
// extern uint16_t generic_access_service_handle;
// extern uint16_t name_characteristic_handle;
// extern uint16_t time_sync_characteristic_handle;
// extern uint16_t json_sync_characteristic_handle;
// extern uint16_t json_delete_characteristic_handle;

extern volatile bool rtc_update_requested;
extern uint32_t receivedEpoch;

extern volatile bool json_requested;

extern volatile bool delete_requested;

extern void syncRtcNotification();
extern void sendJsonToCentral(String json);
extern void deleteAllJsonFiles();