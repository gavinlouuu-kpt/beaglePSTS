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
#include <credential.h>
#include <time.h>
#include <saveData.h>
#include <map>
#include <vector>
#include <beagleCLI.h>
#include <Network.h>


std::map<String, std::function<void()>> commandMap;

#include <Firebase_ESP_Client.h>

// #include <addons/TokenHelper.h>
WiFiManager wifiManager;


void setup()
{
    Serial.begin(115200);
    
    cmdSetup();
    
    
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        Serial.println("Formatting LittleFS due to error...");
        if (LittleFS.format()) {
            Serial.println("LittleFS formatted successfully");
        } else {
            Serial.println("Formatting LittleFS failed");
        }
        return;
    }

}

void loop()
{
    bool status = WiFi.status();
    if (WiFi.status() != WL_CONNECTED) {
        wifiManager.ManageWIFI();
        firebaseSetup();
    }
    else {
        
        beagleCLI();
    }
    
    
}