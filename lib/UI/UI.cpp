#include "UI.h"
#include "Arduino.h"
#include "WiFi.h"
#include "lvgl.h"
#include <vector>
#include "time.h"
#include "TFT_eSPI.h"
#include "saveData.h"
#include "SensorDataFactory.h"
#include <Init.h>
#include "time.h"
// const char *ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 8 * 60 * 60;  // Set your timezone here
// const int daylightOffset_sec = 0;

#include <EEPROM.h>
#define EEPROM_SIZE 128
#define EEPROM_ADDR_WIFI_FLAG 0
#define EEPROM_ADDR_WIFI_CREDENTIAL 4

typedef enum {
  NONE,
  NETWORK_SEARCHING,
  NETWORK_CONNECTED_POPUP,
  NETWORK_CONNECTED,
  NETWORK_CONNECT_FAILED
} Network_Status_t;
Network_Status_t networkStatus = NONE;

lv_obj_t *spinbox;

lv_style_t border_style;
lv_style_t popupBox_style;
lv_obj_t *timeLabel;
lv_obj_t *settings;
lv_obj_t *tuning;
lv_obj_t *settingBtn;
lv_obj_t *testingBtn;
lv_obj_t *tuningBtn;
lv_obj_t *db_settingBtn;
lv_obj_t *spinner_warm;
lv_obj_t *spinner_huff;
lv_obj_t *spinner_save;
lv_obj_t *settingCloseBtn;
lv_obj_t *tuningCloseBtn;
lv_obj_t *settingWiFiSwitch;
lv_obj_t *wfList;
lv_obj_t *settinglabel;
lv_obj_t *tuninglabel;
lv_obj_t *mboxConnect;
lv_obj_t *mboxTitle;
lv_obj_t *mboxPassword;
lv_obj_t *mboxConnectBtn;
lv_obj_t *mboxCloseBtn;
lv_obj_t *keyboard;
lv_obj_t *popupBox;
lv_obj_t *popupBoxCloseBtn;
lv_timer_t *timer;

int foundNetworks = 0;
unsigned long networkTimeout = 10 * 1000;
String ssidName, ssidPW;

TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler, ntCheckTaskHandler;
std::vector<String> foundWifiList;

void tryPreviousNetwork() {

  if (!EEPROM.begin(EEPROM_SIZE)) {
    delay(1000);
    ESP.restart();
  }

  loadWIFICredentialEEPROM();
}

void saveWIFICredentialEEPROM(int flag, String ssidpw) {
  EEPROM.writeInt(EEPROM_ADDR_WIFI_FLAG, flag);
  EEPROM.writeString(EEPROM_ADDR_WIFI_CREDENTIAL, flag == 1 ? ssidpw : "");
  EEPROM.commit();
}

void loadWIFICredentialEEPROM() {
  int wifiFlag = EEPROM.readInt(EEPROM_ADDR_WIFI_FLAG);
  String wifiCredential = EEPROM.readString(EEPROM_ADDR_WIFI_CREDENTIAL);

  if (wifiFlag == 1 && wifiCredential.length() != 0 && wifiCredential.indexOf(" ") != -1) {
    char preSSIDName[30], preSSIDPw[30];
    if (sscanf(wifiCredential.c_str(), "%s %s", preSSIDName, preSSIDPw) == 2) {

      lv_obj_add_state(settingWiFiSwitch, LV_STATE_CHECKED);
      lv_event_send(settingWiFiSwitch, LV_EVENT_VALUE_CHANGED, NULL);

      popupMsgBox("Welcome Back!", "Attempts to reconnect to the previously connected network.");
      ssidName = String(preSSIDName);
      ssidPW = String(preSSIDPw);
      networkConnector();
    } else {
      saveWIFICredentialEEPROM(0, "");
      // ESP.restart();
    }
  }
}

void setStyle() {
  lv_style_init(&border_style);
  lv_style_set_border_width(&border_style, 2);
  lv_style_set_border_color(&border_style, lv_color_black());

  lv_style_init(&popupBox_style);
  lv_style_set_radius(&popupBox_style, 10);
  lv_style_set_bg_opa(&popupBox_style, LV_OPA_COVER);
  lv_style_set_border_color(&popupBox_style, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_border_width(&popupBox_style, 5);
}

void buildStatusBar() {

lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, lv_color_hex(0xC5C5C5));
  lv_style_set_bg_opa(&style_btn, LV_OPA_50);

  lv_obj_t *statusBar = lv_obj_create(lv_scr_act());
  lv_obj_set_size(statusBar, tft.width(), 30);
  lv_obj_align(statusBar, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_remove_style(statusBar, NULL, LV_PART_SCROLLBAR | LV_STATE_ANY);

  timeLabel = lv_label_create(statusBar);
  lv_obj_set_size(timeLabel, tft.width() - 50, 30);

  lv_label_set_text(timeLabel, "WiFi Not Connected!  ");
  lv_obj_align(timeLabel, LV_ALIGN_LEFT_MID, 2, 7);

  settingBtn = lv_btn_create(statusBar);
  lv_obj_set_size(settingBtn, 30, 30);
  lv_obj_align(settingBtn, LV_ALIGN_RIGHT_MID, 0, 0);

  testingBtn = lv_btn_create(statusBar);
  lv_obj_set_size(testingBtn, 30, 30);
  lv_obj_align(testingBtn, LV_ALIGN_RIGHT_MID, -35, 0);

  tuningBtn = lv_btn_create(statusBar);
  lv_obj_set_size(tuningBtn, 30, 30);
  lv_obj_align(tuningBtn, LV_ALIGN_RIGHT_MID, -70, 0);

  // db_settingBtn = lv_btn_create(statusBar);
  // lv_obj_set_size(db_settingBtn, 30, 30);
  // lv_obj_align(db_settingBtn, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_add_event_cb(tuningBtn, tuning_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *label_tuning = lv_label_create(tuningBtn); /*Add a label to the button*/
  lv_label_set_text(label_tuning, LV_SYMBOL_OK);  /*Set the labels text*/
  lv_obj_center(label_tuning);
  
  lv_obj_add_event_cb(testingBtn, test_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *label_test = lv_label_create(testingBtn); /*Add a label to the button*/
  lv_label_set_text(label_test, LV_SYMBOL_WARNING);  /*Set the labels text*/
  lv_obj_center(label_test);

  lv_obj_add_event_cb(settingBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *label = lv_label_create(settingBtn); /*Add a label to the button*/
  lv_label_set_text(label, LV_SYMBOL_SETTINGS);  /*Set the labels text*/
  lv_obj_center(label);
}

void tuning_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    if (btn == tuningBtn){
      lv_obj_clear_flag(tuning, LV_OBJ_FLAG_HIDDEN);
    } else if (btn == tuningCloseBtn) {
      lv_obj_add_flag(tuning, LV_OBJ_FLAG_HIDDEN);
    }
  }
}
//-----test
void testBreath() {
  xTaskCreate(test_check_breath,
              "checkTask",
              2048,
              NULL,
              1,
              NULL);
}

void test_check_breath(void *pvParameters) {
  SensorDataFactory sensorDataFactory;
  ledcWrite(PumpPWM, 155); // turn on pump
  int breath = sensorDataFactory.breath_check();
  ledcWrite(PumpPWM, 0); // turn off pump
  vTaskDelete(NULL);
}

void test_btn_event_cb(lv_event_t *e){
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED && btn == testingBtn) {
    testBreath();
    // testFactory.dataStream();
  }
}
//----- test
void btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    if (btn == settingBtn) {
      lv_obj_clear_flag(settings, LV_OBJ_FLAG_HIDDEN);
    } else if (btn == settingCloseBtn) {
      lv_obj_add_flag(settings, LV_OBJ_FLAG_HIDDEN);
    } else if (btn == mboxConnectBtn) {
      ssidPW = String(lv_textarea_get_text(mboxPassword));

      networkConnector();
      lv_obj_move_background(mboxConnect);
      popupMsgBox("Connecting!", "Attempting to connect to the selected network.");
      lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN); // Hide mboxConnect after attempting to connect
    } else if (btn == mboxCloseBtn) {
      lv_obj_move_background(mboxConnect); //Potentially not needed as mbox is hidden
      lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN); // Hide mboxConnect when canceling
    } else if (btn == popupBoxCloseBtn) {
      lv_obj_move_background(popupBox);
    }

  } else if (code == LV_EVENT_VALUE_CHANGED) {
    if (btn == settingWiFiSwitch) {

      if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {

        if (ntScanTaskHandler == NULL) {
          networkStatus = NETWORK_SEARCHING;
          networkScanner();
          timer = lv_timer_create(timerForNetwork, 1000, wfList);
          lv_list_add_text(wfList, "WiFi: Looking for Networks...");
        }

      } else {

        if (ntScanTaskHandler != NULL) {
          networkStatus = NONE;
          vTaskDelete(ntScanTaskHandler);
          ntScanTaskHandler = NULL;
          lv_timer_del(timer);
          lv_obj_clean(wfList);
        }

        if (WiFi.status() == WL_CONNECTED) {
          WiFi.disconnect(true);
          lv_label_set_text(timeLabel, "WiFi Not Connected!    " LV_SYMBOL_CLOSE);
        }
      }
    }
  }
}

void networkCheck(){
  xTaskCreate(
    wifiCheckTask,       /* Task function */
    "WiFiCheckTask",     /* Name of the task */
    2048,                /* Stack size */
    NULL,                /* Task input parameter */
    1,                   /* Priority of the task */
    &ntCheckTaskHandler);               /* Task handle */
}

void wifiCheckTask(void *pvParameters) {
  for (;;) {
    if (WiFi.status() != WL_CONNECTED) {
      // Serial.println("WiFi connection lost.");
      // WiFi.disconnect();
    } else {
      // networkStatus = NETWORK_CONNECTED; // Confirm that we are still connected
      fbKeepAlive();
    }
    vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds
  }
}

void timerForNetwork(lv_timer_t *timer) {
  LV_UNUSED(timer);
  // Serial.println(networkStatus);

  switch (networkStatus) {

    case NETWORK_SEARCHING:
      showingFoundWiFiList();
      break;

    case NETWORK_CONNECTED_POPUP:
      popupMsgBox("WiFi Connected!", "Now you'll get the current time soon.");
      networkStatus = NETWORK_CONNECTED;
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer,"asia.pool.ntp.org","time.nist.gov");
      break;

    case NETWORK_CONNECTED:

      showingFoundWiFiList();
      updateLocalTime();
      break;

    case NETWORK_CONNECT_FAILED:
      networkStatus = NETWORK_SEARCHING;
      popupMsgBox("Oops!", "Please check your wifi password and try again.");
      break;

    default:
      break;
  }
}

void showingFoundWiFiList() {
  if (foundWifiList.size() == 0 || foundNetworks == foundWifiList.size())
    return;

  lv_obj_clean(wfList);
  lv_list_add_text(wfList, foundWifiList.size() > 1 ? "WiFi: Found Networks" : "WiFi: Not Found!");

  for (std::vector<String>::iterator item = foundWifiList.begin(); item != foundWifiList.end(); ++item) {
    lv_obj_t *btn = lv_list_add_btn(wfList, LV_SYMBOL_WIFI, (*item).c_str());
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);
    delay(1);
  }

  foundNetworks = foundWifiList.size();
}



void buildBody() {
  lv_obj_t *bodyScreen = lv_obj_create(lv_scr_act());
  lv_obj_add_style(bodyScreen, &border_style, 0);
  lv_obj_set_size(bodyScreen, tft.width(), tft.height() - 34); //-34
  lv_obj_align(bodyScreen, LV_ALIGN_BOTTOM_MID, 0, 0);


  spinner_warm = lv_spinner_create(lv_scr_act(), 1000, 50);
  lv_obj_add_event_cb(spinner_warm, db_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_set_size(spinner_warm, 70, 70);
  lv_obj_align(spinner_warm, LV_ALIGN_CENTER, -90, 5);
  lv_obj_add_flag(spinner_warm, LV_OBJ_FLAG_HIDDEN);
  // warm label
  lv_obj_t *label_warm = lv_label_create(bodyScreen); /*Add a label to the spinner*/
  lv_label_set_text(label_warm, "1. Ready");  /*Set the labels text*/
  lv_obj_align(label_warm, LV_ALIGN_CENTER, -90, -75);
  
  spinner_huff = lv_spinner_create(lv_scr_act(), 1000, 50);
  lv_obj_add_event_cb(spinner_huff, db_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_set_size(spinner_huff, 70, 70);
  lv_obj_align(spinner_huff, LV_ALIGN_CENTER, 0, 5);
  lv_obj_add_flag(spinner_huff, LV_OBJ_FLAG_HIDDEN);
  // huff label
  lv_obj_t *label_huff = lv_label_create(bodyScreen); /*Add a label to the spinner*/
  lv_label_set_text(label_huff, "2. Huff");  /*Set the labels text*/
  lv_obj_align(label_huff, LV_ALIGN_CENTER, 0, -75);

  spinner_save = lv_spinner_create(lv_scr_act(), 1000, 50);
  lv_obj_add_event_cb(spinner_save, db_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_set_size(spinner_save, 70, 70);
  lv_obj_align(spinner_save, LV_ALIGN_CENTER, 90, 5);
  lv_obj_add_flag(spinner_save, LV_OBJ_FLAG_HIDDEN);
  // save label
  lv_obj_t *label_save = lv_label_create(bodyScreen); /*Add a label to the button*/
  lv_label_set_text(label_save, "3. Save");  /*Set the labels text*/
  lv_obj_align(label_save, LV_ALIGN_CENTER, 90, -75);

  // action button
  db_settingBtn = lv_btn_create(bodyScreen);
  lv_obj_set_size(db_settingBtn, 60, 30);
  lv_obj_align(db_settingBtn, LV_ALIGN_CENTER, 0, 60);
  
  lv_obj_add_event_cb(db_settingBtn, db_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *label = lv_label_create(db_settingBtn); /*Add a label to the button*/
  lv_label_set_text(label, LV_SYMBOL_OK);  /*Set the labels text*/
  lv_obj_center(label);
}


void db_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED && btn == db_settingBtn) {
    
    firestoreUpload(); // Assume this is now non-blocking or called asynchronously
    
  }

}

void check_upload_status(lv_timer_t * timer) {
    // You don't necessarily need to use the timer parameter if not needed
    (void)timer; // This line is just to avoid unused variable warnings

    if (warmingInProgress) {
        lv_obj_clear_flag(spinner_warm, LV_OBJ_FLAG_HIDDEN); // Show spinner_huff
    } else {
        lv_obj_add_flag(spinner_warm, LV_OBJ_FLAG_HIDDEN); // Hide spinner_huff after the operation
    }
    if (samplingInProgress) {
        lv_obj_clear_flag(spinner_huff, LV_OBJ_FLAG_HIDDEN); // Show spinner_huff
    } else {
        lv_obj_add_flag(spinner_huff, LV_OBJ_FLAG_HIDDEN); // Hide spinner_huff after the operation
    }
    if (uploadInProgress) {
        lv_obj_clear_flag(spinner_save, LV_OBJ_FLAG_HIDDEN); // Show spinner_huff
    } else {
        lv_obj_add_flag(spinner_save, LV_OBJ_FLAG_HIDDEN); // Hide spinner_huff after the operation
    }
}
  

void buildSettings() {
  settings = lv_obj_create(lv_scr_act());
  lv_obj_add_style(settings, &border_style, 0);
  lv_obj_set_size(settings, tft.width() - 100, tft.height() - 40);
  lv_obj_align(settings, LV_ALIGN_TOP_RIGHT, -20, 20);

  settinglabel = lv_label_create(settings);
  lv_label_set_text(settinglabel, "Settings " LV_SYMBOL_SETTINGS);
  lv_obj_align(settinglabel, LV_ALIGN_TOP_LEFT, 0, 0);

  settingCloseBtn = lv_btn_create(settings);
  lv_obj_set_size(settingCloseBtn, 30, 30);
  lv_obj_align(settingCloseBtn, LV_ALIGN_TOP_RIGHT, 0, -10);
  lv_obj_add_event_cb(settingCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *btnSymbol = lv_label_create(settingCloseBtn);
  lv_label_set_text(btnSymbol, LV_SYMBOL_CLOSE);
  lv_obj_center(btnSymbol);

  settingWiFiSwitch = lv_switch_create(settings);
  lv_obj_add_event_cb(settingWiFiSwitch, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align_to(settingWiFiSwitch, settinglabel, LV_ALIGN_TOP_RIGHT, 60, -10);
  lv_obj_add_flag(settings, LV_OBJ_FLAG_HIDDEN);

  wfList = lv_list_create(settings);
  lv_obj_set_size(wfList, tft.width() - 140, 210);
  lv_obj_align_to(wfList, settinglabel, LV_ALIGN_TOP_LEFT, 0, 30);
}

void buildTuning() {
  tuning = lv_obj_create(lv_scr_act());
  lv_obj_add_style(tuning, &border_style, 0);
  lv_obj_set_size(tuning, tft.width() - 100, tft.height() - 40);
  lv_obj_align(tuning, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_flag(tuning, LV_OBJ_FLAG_HIDDEN);

  tuninglabel = lv_label_create(tuning);
  lv_label_set_text(tuninglabel, "Tuning " LV_SYMBOL_OK);
  lv_obj_align(tuninglabel, LV_ALIGN_TOP_LEFT, 0, 0);

  tuningCloseBtn = lv_btn_create(tuning);
  lv_obj_set_size(tuningCloseBtn, 30, 30);
  lv_obj_align(tuningCloseBtn, LV_ALIGN_TOP_RIGHT, 0, -10);
  lv_obj_add_event_cb(tuningCloseBtn, tuning_btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *btnSymbol = lv_label_create(tuningCloseBtn);
  lv_label_set_text(btnSymbol, LV_SYMBOL_CLOSE);
  lv_obj_center(btnSymbol);

  // settingWiFiSwitch = lv_switch_create(tuning);
  // lv_obj_add_event_cb(settingWiFiSwitch, btn_event_cb, LV_EVENT_ALL, NULL);
  // lv_obj_align_to(settingWiFiSwitch, tuninglabel, LV_ALIGN_TOP_RIGHT, 60, -10);
  // lv_obj_add_flag(tuning, LV_OBJ_FLAG_HIDDEN);

  // wfList = lv_list_create(tuning);
  // lv_obj_set_size(wfList, tft.width() - 140, 210);
  // lv_obj_align_to(wfList, tuninglabel, LV_ALIGN_TOP_LEFT, 0, 30);
  lv_example_spinbox_1();
}

static void lv_spinbox_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox);
    }
}

static void lv_spinbox_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(spinbox);
    }
}


void lv_example_spinbox_1(void)
{
    spinbox = lv_spinbox_create(tuning);
    lv_spinbox_set_range(spinbox, 0, 255);
    lv_spinbox_set_digit_format(spinbox, 3,3);
    lv_spinbox_step_prev(spinbox);
    lv_obj_set_width(spinbox, 100);
    lv_obj_center(spinbox);

    lv_coord_t h = lv_obj_get_height(spinbox);

    lv_obj_t * btn = lv_btn_create(tuning);
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, LV_EVENT_ALL,  NULL);

    btn = lv_btn_create(tuning);
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, NULL);
}

void list_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);


  if (code == LV_EVENT_CLICKED) {

    String selectedItem = String(lv_list_get_btn_text(wfList, obj));
    for (int i = 0; i < selectedItem.length() - 1; i++) {
      if (selectedItem.substring(i, i + 2) == " (") {
        ssidName = selectedItem.substring(0, i);
        lv_label_set_text_fmt(mboxTitle, "Selected WiFi SSID: %s", ssidName);
        // reveal mboxConnect when a SSID item is clicked
        lv_obj_clear_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(mboxConnect);
        break;
      }
    }
  }
}

/*
 * NETWORK TASKS
 */

void networkScanner() {
  xTaskCreate(scanWIFITask,
              "ScanWIFITask",
              4096,
              NULL,
              1,
              &ntScanTaskHandler);
}

void networkConnector() {
  xTaskCreate(beginWIFITask,
              "beginWIFITask",
              8192, //original 2048
              NULL,
              1,
              &ntConnectTaskHandler);
}



void scanWIFITask(void *pvParameters) {
  while (1) {
    foundWifiList.clear();
    int n = WiFi.scanNetworks();
    vTaskDelay(10);
    for (int i = 0; i < n; ++i) {
      String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      foundWifiList.push_back(item);
      vTaskDelay(10);
    }
    vTaskDelay(5000);
  }
}


void beginWIFITask(void *pvParameters) {

  unsigned long startingTime = millis();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  vTaskDelay(100);

  WiFi.begin(ssidName.c_str(), ssidPW.c_str());
  while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < networkTimeout) {
    vTaskDelay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    networkStatus = NETWORK_CONNECTED_POPUP;
    saveWIFICredentialEEPROM(1, ssidName + " " + ssidPW);
    firebaseSetup();
  } else {
    networkStatus = NETWORK_CONNECT_FAILED;
    saveWIFICredentialEEPROM(0, "");
  }

  vTaskDelete(NULL);
}

void buildPWMsgBox() {

  mboxConnect = lv_obj_create(lv_scr_act());
  lv_obj_add_style(mboxConnect, &border_style, 0);
  lv_obj_set_size(mboxConnect, tft.width() * 3 / 4, tft.height() / 2);
  lv_obj_align(mboxConnect, LV_ALIGN_CENTER, 0, -60); // Move 50 pixels upwards from the center

  // lv_obj_center(mboxConnect);

  mboxTitle = lv_label_create(mboxConnect);
  lv_label_set_text(mboxTitle, "Selected WiFi SSID: ThatProject");
  lv_obj_align(mboxTitle, LV_ALIGN_TOP_LEFT, 0, 0);

  mboxPassword = lv_textarea_create(mboxConnect);
  lv_obj_set_size(mboxPassword, tft.width() / 2, 40);
  lv_obj_align_to(mboxPassword, mboxTitle, LV_ALIGN_TOP_LEFT, 0, 30);
  lv_textarea_set_placeholder_text(mboxPassword, "Password?");
  lv_obj_add_event_cb(mboxPassword, text_input_event_cb, LV_EVENT_ALL, keyboard);

  mboxConnectBtn = lv_btn_create(mboxConnect);
  lv_obj_add_event_cb(mboxConnectBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align(mboxConnectBtn, LV_ALIGN_BOTTOM_LEFT, 0, 15);
  lv_obj_t *btnLabel = lv_label_create(mboxConnectBtn);
  lv_label_set_text(btnLabel, "Connect");
  lv_obj_center(btnLabel);

  mboxCloseBtn = lv_btn_create(mboxConnect);
  lv_obj_add_event_cb(mboxCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align(mboxCloseBtn, LV_ALIGN_BOTTOM_RIGHT, 0, 15);
  lv_obj_t *btnLabel2 = lv_label_create(mboxCloseBtn);
  lv_label_set_text(btnLabel2, "Cancel");
  lv_obj_center(btnLabel2);
  // Adding the hidden flag to mboxConnect
  lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
}

void text_input_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);

  if (code == LV_EVENT_FOCUSED) {
    lv_obj_move_foreground(keyboard);
    lv_keyboard_set_textarea(keyboard, ta);
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  }

  if (code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(keyboard, NULL);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  }
}

void makeKeyboard() {
  keyboard = lv_keyboard_create(lv_scr_act());
  // lv_obj_set_size(keyboard, tft.width(), tft.height() / 2);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

void popupMsgBox(String title, String msg) {

  if (popupBox != NULL) {
    lv_obj_del(popupBox);
  }

  popupBox = lv_obj_create(lv_scr_act());
  lv_obj_add_style(popupBox, &popupBox_style, 0);
  lv_obj_set_size(popupBox, tft.width() * 2 / 3, tft.height() / 2);
  lv_obj_center(popupBox);

  lv_obj_t *popupTitle = lv_label_create(popupBox);
  lv_label_set_text(popupTitle, title.c_str());
  lv_obj_set_width(popupTitle, tft.width() * 2 / 3 - 50);
  lv_obj_align(popupTitle, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t *popupMSG = lv_label_create(popupBox);
  lv_obj_set_width(popupMSG, tft.width() * 2 / 3 - 50);
  lv_label_set_text(popupMSG, msg.c_str());
  lv_obj_align(popupMSG, LV_ALIGN_TOP_LEFT, 0, 40);

  popupBoxCloseBtn = lv_btn_create(popupBox);
  lv_obj_add_event_cb(popupBoxCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align(popupBoxCloseBtn, LV_ALIGN_TOP_RIGHT, 0, 0);
  lv_obj_t *btnLabel = lv_label_create(popupBoxCloseBtn);
  lv_label_set_text(btnLabel, "Okay");
  lv_obj_center(btnLabel);
}

void updateLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  char hourMin[6];
  strftime(hourMin, 6, "%H:%M", &timeinfo);
  String hourMinWithSymbol = String(hourMin);
  hourMinWithSymbol += "   ";
  hourMinWithSymbol += LV_SYMBOL_WIFI;
  lv_label_set_text(timeLabel, hourMinWithSymbol.c_str());
}

