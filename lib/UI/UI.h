#ifndef UI_h
#define UI_h

#include "TFT_eSPI.h"
#include <lvgl.h>

extern TFT_eSPI tft;
extern lv_obj_t *save_success;
extern lv_obj_t *save_fail;

void tryPreviousNetwork();
void saveWIFICredentialEEPROM(int flag, String ssidpw);
void loadWIFICredentialEEPROM();
void setStyle();
void buildStatusBar();
void btn_event_cb(lv_event_t *e);
void db_btn_event_cb(lv_event_t *e);
void sampling_btn_event_cb(lv_event_t *e);
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
void toggle_visibility(lv_obj_t* obj, bool condition);

void networkCheck();
void wifiCheckTask(void *pvParameters);

void check_upload_status(lv_timer_t *timer);
void updateLocalTime();


void test_btn_event_cb(lv_event_t *e);
void test_check_breath(void *pvParameters);
void testBreath();

void tuning_btn_event_cb(lv_event_t *e);
void buildTuning();

static void lv_spinbox_increment_event_cb(lv_event_t * e);
static void lv_spinbox_decrement_event_cb(lv_event_t * e);
void lv_example_spinbox_1(void);

static void resetUploadState(lv_timer_t * timer);

#endif // UI_h