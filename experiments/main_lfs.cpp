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
#include <LITTLEFS.h>
#include <SD.h>
#include <WiFi.h>
#include <Update.h>


// void setup()
// {
//     Serial.begin(115200);
    
//     if (!LITTLEFS.begin()) {
//         Serial.println("An Error has occurred while mounting LittleFS");
//         return;
//     }
//     Serial.println("LittleFS mounted successfully");
    
// }

// void loop()
// {
// }

void setup()
{
    Serial.begin(115200);
    
    if (!LITTLEFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        Serial.println("Formatting LittleFS due to error...");
        if (LITTLEFS.format()) {
            Serial.println("LittleFS formatted successfully");
        } else {
            Serial.println("Formatting LittleFS failed");
        }
        return;
    }
    Serial.println("LittleFS mounted successfully");
}

void loop(){
    
}