#include "RTClib.h"
#include "bleHandler.h"
#include "jsonHandler.h"

RTC_DS3231 rtc;

volatile bool rtc_update_requested = false;
volatile bool json_requested = false;
volatile bool delete_requested = false;

uint32_t receivedEpoch;

static void ble_initialize_gatt_db();
static void ble_start_advertising();

// UUID: 930788e7-5e3d-7c7d-65ff-2461a6023d44
const uuid_128 my_service_uuid = {
  .data = { 0x44, 0x3d, 0x02, 0xa6, 0x61, 0x24, 0xff, 0x65, 0x7d, 0x7c, 0x3d, 0x5e, 0xe7, 0x88, 0x07, 0x93 }
};

// UUID: 9cd2702a-656d-539a-d060-c341a485a861
const uuid_128 time_sync_characteristic_uuid = {
  .data = { 0x61, 0xa8, 0x85, 0xa4, 0x41, 0xc3, 0x60, 0xd0, 0x9a, 0x53, 0x6d, 0x65, 0x2a, 0x70, 0xd2, 0x9c }
};

// UUID: f447c752-5b63-3cb9-d388-b305e02c19f3
const uuid_128 json_sync_characteristic_uuid = {
  .data = {0xf3, 0x19, 0x2c, 0xe0, 0x05, 0xb3, 0x88, 0xd3, 0xb9, 0x3c, 0x63, 0x5b, 0x52, 0xc7, 0x47, 0xf4 }
};

// UUID: 528fe50b-7898-66dd-9a41-372fe61744fd
const uuid_128 json_delete_characteristic_uuid = {
  .data = { 0xfd, 0x44, 0x17, 0xe6, 0x2f, 0x37, 0x41, 0x9a, 0xdd, 0x66, 0x98, 0x78, 0x0b, 0xe5, 0x8f, 0x52 }
};

static const uint8_t advertised_name[] = "XIAO_MG24 Server";  // Name of your BLE device
uint16_t gattdb_session_id;
uint16_t my_service_handle;
uint16_t generic_access_service_handle;
uint16_t name_characteristic_handle;
static uint16_t time_sync_characteristic_handle;
static uint16_t json_sync_characteristic_handle;
static uint16_t json_delete_characteristic_handle;

/**************************************************************************/ /**
 * Bluetooth stack event handler
 * Called when an event happens on BLE the stack
 *
 * @param[in] evt Event coming from the Bluetooth stack
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt) {
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      {
        Serial.println("BLE stack booted");

        // Initialize the application specific GATT table
        ble_initialize_gatt_db();

        // Start advertising
        ble_start_advertising();
        Serial.println("BLE advertisement started");
      }
      break;

    // -------------------------------
    // This event indicates that a new connection was opened
    case sl_bt_evt_connection_opened_id:
      Serial.println("BLE connection opened");
      break;

    // -------------------------------
    // This event indicates that a connection was closed
    case sl_bt_evt_connection_closed_id:
      Serial.println("BLE connection closed");
      // Restart the advertisement
      ble_start_advertising();
      Serial.println("BLE advertisement restarted");
      break;

    // -------------------------------
    // This event is received when a GATT characteristic status changes
    case sl_bt_evt_gatt_server_attribute_value_id:
     
      // 時計を同期
      if (time_sync_characteristic_handle ==
            evt->data.evt_gatt_server_attribute_value.attribute) {

        auto *value = &evt->data.evt_gatt_server_attribute_value.value;

        Serial.print("Received length: ");
        Serial.println(value->len);

        if(value->len == 4){
            memcpy(
              &receivedEpoch,
              value->data,
              sizeof(uint32_t)
            );

            Serial.print("UNIX TIME:");
            Serial.println(receivedEpoch);
            rtc_update_requested = true;
        }
      }

      // JSONを同期
      if(json_sync_characteristic_handle == evt->data.evt_gatt_server_attribute_value.attribute){

        auto *value = &evt->data.evt_gatt_server_attribute_value.value;

        Serial.println("Recieve JSON Subscription");
        Serial.print("Received length: ");
        Serial.println(value->len);
        
        if(value->len == 8){
            json_requested = true;
            Serial.println("Requested Json.");
        }
      }

      // JSONを削除
      if(json_delete_characteristic_handle == evt->data.evt_gatt_server_attribute_value.attribute){

        auto *value = &evt->data.evt_gatt_server_attribute_value.value;

        Serial.println("Recieve JSON DELETE Commannd.");
        Serial.print("Received length: ");
        Serial.println(value->len);
        
        if(value->len == 6){
            delete_requested = true;
            Serial.println("Requested Json Delete.");
        }
      }
    break;

    // -------------------------------
    // Default event handler
    default:
      break;
  }
}


/**************************************************************************/ /**
 * Starts BLE advertisement
 * Initializes advertising if it's called for the first time
 *****************************************************************************/
static void ble_start_advertising() {
  static uint8_t advertising_set_handle = 0xff;
  static bool init = true;
  sl_status_t sc;

  if (init) {
    // Create an advertising set
    sc = sl_bt_advertiser_create_set(&advertising_set_handle);
    app_assert_status(sc);

    // Set advertising interval to 100ms
    sc = sl_bt_advertiser_set_timing(
      advertising_set_handle,
      160,  // minimum advertisement interval (milliseconds * 1.6)
      160,  // maximum advertisement interval (milliseconds * 1.6)
      0,    // advertisement duration
      0);   // maximum number of advertisement events
    app_assert_status(sc);

    init = false;
  }

  // Generate data for advertising
  sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle, sl_bt_advertiser_general_discoverable);
  app_assert_status(sc);

  // Start advertising and enable connections
  sc = sl_bt_legacy_advertiser_start(advertising_set_handle, sl_bt_advertiser_connectable_scannable);
  app_assert_status(sc);
}

/**************************************************************************/ /**
 * Initializes the GATT database
 * Creates a new GATT session and adds certain services and characteristics
 *****************************************************************************/
static void ble_initialize_gatt_db() {
  sl_status_t sc;
  // Create a new GATT database
  sc = sl_bt_gattdb_new_session(&gattdb_session_id);
  app_assert_status(sc);

  // Add the Generic Access service to the GATT DB
  const uint8_t generic_access_service_uuid[] = { 0x00, 0x18 };
  sc = sl_bt_gattdb_add_service(gattdb_session_id,
                                sl_bt_gattdb_primary_service,
                                SL_BT_GATTDB_ADVERTISED_SERVICE,
                                sizeof(generic_access_service_uuid),
                                generic_access_service_uuid,
                                &generic_access_service_handle);
  app_assert_status(sc);

  // Add the Device Name characteristic to the Generic Access service
  // The value of the Device Name characteristic will be advertised
  const sl_bt_uuid_16_t device_name_characteristic_uuid = { .data = { 0x00, 0x2A } };
  sc = sl_bt_gattdb_add_uuid16_characteristic(gattdb_session_id,
                                              generic_access_service_handle,
                                              SL_BT_GATTDB_CHARACTERISTIC_READ,
                                              0x00,
                                              0x00,
                                              device_name_characteristic_uuid,
                                              sl_bt_gattdb_fixed_length_value,
                                              sizeof(advertised_name) - 1,
                                              sizeof(advertised_name) - 1,
                                              advertised_name,
                                              &name_characteristic_handle);
  app_assert_status(sc);

  // Start the Generic Access service
  sc = sl_bt_gattdb_start_service(gattdb_session_id, generic_access_service_handle);
  app_assert_status(sc);

  // Add my BLE service to the GATT DB
  sc = sl_bt_gattdb_add_service(gattdb_session_id,
                                sl_bt_gattdb_primary_service,
                                SL_BT_GATTDB_ADVERTISED_SERVICE,
                                sizeof(my_service_uuid),
                                my_service_uuid.data,
                                &my_service_handle);
  app_assert_status(sc);


  // Start the Blinky service
  sc = sl_bt_gattdb_start_service(gattdb_session_id, my_service_handle);
  app_assert_status(sc);

  //RTCを同期
  uint8_t time_init_value[1] = {0};

  sc = sl_bt_gattdb_add_uuid128_characteristic(
    gattdb_session_id,
    my_service_handle,

    SL_BT_GATTDB_CHARACTERISTIC_READ |
    SL_BT_GATTDB_CHARACTERISTIC_WRITE |
    SL_BT_GATTDB_CHARACTERISTIC_WRITE_NO_RESPONSE |
    SL_BT_GATTDB_CHARACTERISTIC_NOTIFY,

    0x00,
    0x00,

    time_sync_characteristic_uuid,

    // variable length
    sl_bt_gattdb_variable_length_value,

    32, // max

    1,  // initial length
    time_init_value,

    &time_sync_characteristic_handle
  );

  app_assert_status(sc);


  // JSONをセントラルデバイスに送信
  uint8_t json_char_init_value = 0;
  sc = sl_bt_gattdb_add_uuid128_characteristic(
    gattdb_session_id,
    my_service_handle,
    SL_BT_GATTDB_CHARACTERISTIC_READ |
    SL_BT_GATTDB_CHARACTERISTIC_WRITE |
    SL_BT_GATTDB_CHARACTERISTIC_WRITE_NO_RESPONSE |
    SL_BT_GATTDB_CHARACTERISTIC_NOTIFY,
    0x00,
    0x00,
    json_sync_characteristic_uuid,
    sl_bt_gattdb_fixed_length_value,
    20,                               // max length
    sizeof(json_char_init_value),  // initial value length
    &json_char_init_value,         // initial value
    &json_sync_characteristic_handle);

  app_assert_status(sc);

  // Start my BLE service
  sc = sl_bt_gattdb_start_service(gattdb_session_id, my_service_handle);
  app_assert_status(sc);

  // Commit the GATT DB changes
  sc = sl_bt_gattdb_commit(gattdb_session_id);
  app_assert_status(sc);
}

//RTC同期の信号をセントラルに送信
void syncRtcNotification(){
  const char request[] = "RTC_SYNC";
  sl_status_t sc =
    sl_bt_gatt_server_notify_all(
      time_sync_characteristic_handle,
      strlen(request),
      (const uint8_t*)request
    );
}

// JSONファイルをセントラルに送信
void sendJsonToCentral(String json){
  const int chunkSize = 4096;
  int length = json.length();
  String chunk = "";
  uint8_t buffer[410];

  for(int i = 0; i < length; i += chunkSize){
    chunk =
      json.substring(
        i,
        min(i + chunkSize, length)
      );
      memset(buffer,0,sizeof(buffer));
      chunk.getBytes(
          buffer,
          sizeof(buffer)
      );

      Serial.print("Send Chunk: ");
      Serial.print(chunk);
      Serial.print(", ");
      Serial.println(i);
    }

    sl_bt_gatt_server_notify_all(
      json_sync_characteristic_handle,
      chunk.length(),
      buffer
    );

    delay(100);
}

// セントラルからDELETEコマンドを受け取ってJSONファイルを削除する
void deleteAllJsonFiles() {
  deleteJsonFromNvm3();
}
