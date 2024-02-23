#ifndef UI_h
#define UI_h

#include "TFT_eSPI.h"
#include <lvgl.h>

extern TFT_eSPI tft;

void tryPreviousNetwork();
void saveWIFICredentialEEPROM(int flag, String ssidpw);
void loadWIFICredentialEEPROM();
void setStyle();
void buildStatusBar();
void btn_event_cb(lv_event_t *e);
void db_btn_event_cb(lv_event_t *e);
void timerForNetwork(lv_timer_t *timer);
void showingFoundWiFiList();
void buildBody();
void buildSettings();
void list_event_handler(lv_event_t *e);
void networkScanner();
void networkConnector();
void scanWIFITask(void *pvParameters);
void beginWIFITask(void *pvParameters);
void buildPWMsgBox();
void text_input_event_cb(lv_event_t *e);
void makeKeyboard();
void popupMsgBox(String title, String msg);


void networkCheck();
void wifiCheckTask(void *pvParameters);

void check_upload_status(lv_timer_t *timer);
void updateLocalTime();


void test_btn_event_cb(lv_event_t *e);
void test_check_breath(void *pvParameters);
void testBreath();

void tuning_btn_event_cb(lv_event_t *e);
void buildTuning();

#endif // UI_h