/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/FirebaseJson
 *
 * Copyright (c) 2023 mobizt
 *
 */

#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h>

#include <FirebaseJson.h>
#include <SD.h>
#include <WiFi.h>
#include <Update.h>
// #include <credential.h>
#include <time.h>
// #include <saveData.h>
#include <map>
#include <vector>
// #include <beagleCLI.h>
// #include <Network.h>
#include <UI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// std::map<String, std::function<void()>> commandMap;

// #include <Firebase_ESP_Client.h>

// WiFiManager wifiManager;

const uint32_t screenWidth = 320;
const uint32_t screenHeight = 240;
lv_disp_draw_buf_t draw_buf;
lv_color_t buf[screenWidth * 10];
lv_style_t label_style;
lv_obj_t *headerLabel;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(3);
//   tft.setBrightness(255);

  lv_init();
//   lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

//   /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );


// // sample code ---
// lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

//     /*Initialize the display*/
//     static lv_disp_drv_t disp_drv;
//     lv_disp_drv_init( &disp_drv );
//     /*Change the following line to your display resolution*/
//     disp_drv.hor_res = screenWidth;
//     disp_drv.ver_res = screenHeight;
//     disp_drv.flush_cb = my_disp_flush;
//     disp_drv.draw_buf = &draw_buf;
//     lv_disp_drv_register( &disp_drv );
// // sample code ---

//   lv_indev_drv_t indev_drv;
//   lv_indev_drv_init(&indev_drv);
//   indev_drv.type = LV_INDEV_TYPE_POINTER;
//   indev_drv.read_cb = my_touchpad_read;
//   lv_indev_drv_register(&indev_drv);

//   setStyle();
//   makeKeyboard();
//   buildStatusBar();
//   buildPWMsgBox();
//   buildBody();
//   buildSettings();

//   tryPreviousNetwork();
}

void loop() {
  lv_timer_handler();
  delay(5);
}