#include <Arduino.h>
#include <Wire.h>
#include <LittleFS.h>
#include <FS.h>
#include <pinConfig.h>
#include <Init.h>
#include <FirebaseJson.h>
#include <SD.h>
#include <WiFi.h>
#include <Update.h>
// #include <credential.h>
#include <time.h>
#include <saveData.h>
#include <map>
#include <vector>
#include <Hardware.h>
#include <beagleCLI.h>
#include <Network.h>
#include <UI.h>


std::map<String, std::function<void()>> commandMap;

#include <Firebase_ESP_Client.h>

WiFiManager wifiManager;

#include "FS.h"
#include <SPI.h>

/*Read the touchpad*/

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed, formatting...");
    // If mounting fails, format LittleFS
    if (LittleFS.format()) {
      Serial.println("LittleFS formatted successfully");
      // Try to mount again after formatting
      if (LittleFS.begin()) {
        Serial.println("LittleFS mounted successfully after format");
      } else {
        Serial.println("LittleFS mount failed after format");
      }
    } else {
      Serial.println("LittleFS format failed");
    }
  } else {
    Serial.println("LittleFS mounted successfully");
  }


    Wire.begin(C_SDA, C_SCL);
    pinSetup();
    pwmSetup();
    configInit();
    cmdSetup();
    wifiManager.ManageWIFI();
    firebaseSetup();

}

void loop()
{
    fbKeepAlive();
    beagleCLI();
    delay(5);
}
