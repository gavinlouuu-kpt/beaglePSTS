#include "saveData.h"
#include <WiFi.h>
#include <FirebaseJson.h>
#include <LittleFS.h>
#include <map>
#include <Arduino.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <Init.h>

#include <SensorData.h>
#include <SensorDataFactory.h>
#include <lvgl.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

// Utility function to ensure all directories in the path exist
void ensureDirectoriesExist(const String& path) {
    String dirPath = "/";
    for (int i = 1; i < path.length(); i++) {
        if (path[i] == '/') {
            // Only attempt to create the directory if it doesn't already exist
            if (!LittleFS.exists(dirPath)) {
                if (!LittleFS.mkdir(dirPath)) {
                    Serial.println("Failed to create directory: " + dirPath);
                    return;
                }
            }
        }
        dirPath += path[i];
    }

    // Create the final directory if it doesn't already exist
    if (!LittleFS.exists(dirPath)) {
        if (!LittleFS.mkdir(dirPath)) {
            Serial.println("Failed to create directory: " + dirPath);
            return;
        }
    }
}

void localSave(String localPath, FirebaseJson sample) {
    // Ensure the path starts with a forward slash
    String fullPath = localPath.startsWith("/") ? localPath : "/" + localPath;
    Serial.print("fullPath:");
    Serial.println(fullPath);
    int lastSlashIndex = fullPath.lastIndexOf('/');
    String dirPath = fullPath.substring(0, lastSlashIndex);
    Serial.print("dirPath:");
    Serial.println(dirPath);


    // Ensure all directories in the path exist
    ensureDirectoriesExist(dirPath);

    Serial.println("Proceed to writing...");

    // Serialize sample JSON to String for writing
    String localData;
    sample.toString(localData, true); // 'false' for compact serialization

    // Write to file
    File file = LittleFS.open(fullPath, "w");
    Serial.println("littleFS opens:");
    Serial.println(fullPath);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(localData);
    file.close();
    Serial.println("File created successfully.");
}




void populateJsonArray(FirebaseJsonArray& jsonArray, int* list, int length) {
    for (int i = 0; i < length; i++) {
        jsonArray.add(list[i]);
    }
}


#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

volatile bool busy;
/* 1. Define the WiFi credentials */
// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;

//create a RTOS task to routinely check if wifi is connected and if not,

void firebaseSetup(){
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    /* Assign the api key (required) */
    config.api_key = API_KEY;
    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
    Firebase.reconnectNetwork(true);

    // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
    // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
    fbdo.setBSSLBufferSize(10240 /* Rx buffer size in bytes from 512 - 16384 */, 10240 /* Tx buffer size in bytes from 512 - 16384 */);

    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);
    
    /* Assign download buffer size in byte */
    // Data to be downloaded will read as multiple chunks with this size, to compromise between speed and memory used for buffering.
    config.fcs.download_buffer_size = 2048;

    Firebase.begin(&config, &auth);

    // You can use TCP KeepAlive in FirebaseData object and tracking the server connection status, please read this for detail.
    // https://github.com/mobizt/Firebase-ESP-Client#about-firebasedata-object
    // fbdo.keepAlive(5, 5, 1);
}

void fbKeepAlive(){
    fbdo.keepAlive(5, 5, 1);
    Firebase.ready();
    // Serial.println(fbdo.httpConnected() ? "firebase connected" : "firebase not connected");
}

// The Firebase Storage download callback function
void fcsDownloadCallback(FCS_DownloadStatusInfo info)
{
    if (info.status == firebase_fcs_download_status_init)
    {
        Serial.printf("Downloading firmware %s (%d)\n", info.remoteFileName.c_str(), info.fileSize);
    }
    else if (info.status == firebase_fcs_download_status_download)
    {
        Serial.printf("Downloaded %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
    }
    else if (info.status == firebase_fcs_download_status_complete)
    {
        Serial.println("Update firmware completed.");
        Serial.println();
        Serial.println("Restarting...\n\n");
        delay(2000);
#if defined(ESP32) || defined(ESP8266)
        ESP.restart();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
        rp2040.restart();
#endif
    }
    else if (info.status == firebase_fcs_download_status_error)
    {
        Serial.printf("Download firmware failed, %s\n", info.errorMsg.c_str());
    }
}

void fbOTA(){
        // If you want to get download url to use with your own OTA update process using core update library,
        // see Metadata.ino example
        Serial.println("\nDownload firmware file...\n");
        Serial.println("STORAGE_BUCKET_ID:"); //debug
        Serial.print(STORAGE_BUCKET_ID);
        Serial.println("firmware.bin"); //debug
        
        // This function will allocate 16k+ memory for internal SSL client.
        if (!Firebase.Storage.downloadOTA(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, "firmware.bin" /* path of firmware file stored in the bucket */, fcsDownloadCallback /* callback function */))
            Serial.println(fbdo.errorReason());
}


template <typename T>
std::string vectorToString(const std::vector<T>& vec, const std::string& delimiter = ",") {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) {
            oss << delimiter; // Use the provided delimiter
        }
        oss << vec[i];
    }
    return oss.str();
}

TaskHandle_t fsUploadTaskHandler;

void firestoreUpload() {
  xTaskCreate(dataFF,
              "dataTask",
              20480,
              NULL,
              1,
              &fsUploadTaskHandler);
}


volatile bool uploadInProgress = false;

UploadState uploadState = NotStarted;


void dataFF(void *pvParameters){
    Serial.println("Enter dataFF");
    
    SensorDataFactory factory;
    SensorData sensorData = factory.createSensorData();
    
    uploadInProgress = true;

    // std::vector<struct firebase_firestore_document_write_t> writes;

    // struct firebase_firestore_document_write_t update_write;
    // update_write.type = firebase_firestore_document_write_type_update;
            
    FirebaseJson content;
    std::string macAddressTest = WiFi.macAddress().c_str();
            
    // obtain date
    // struct tm timeinfo;
    // if (!getLocalTime(&timeinfo)) {
    //     Serial.println("Failed to obtain time");
    //     return;
    // }

    // char today[11]; // Buffer to hold the date string
    // strftime(today, sizeof(today), "%Y_%m_%d", &timeinfo); // Format: YYYY-MM-DD

    // char currentTime[9]; // Buffer to hold the time string
    // strftime(currentTime, sizeof(currentTime), "%H_%M_%S", &timeinfo); // Format: HH:MM:SS

    // std::string jsonKey = std::string("fields/d") + today + "_t" + currentTime;
    std::string jsonKey = std::string("fields/t_") + String(COUNT).c_str();
    std::string info = jsonKey + "/mapValue/fields/info/stringValue";
    std::string condition = jsonKey + "/mapValue/fields/condition/stringValue";
    std::string data_200 = jsonKey + "/mapValue/fields/data_200/stringValue";
    std::string data_300 = jsonKey + "/mapValue/fields/data_300/stringValue";
    std::string data_400 = jsonKey + "/mapValue/fields/data_400/stringValue";
            
    std::string infoString = sensorData.getInfoString();
    std::string conString = vectorToString(sensorData.getConVec()); 
    std::string dataString_200 = vectorToString(sensorData.getDataVec200()); 
    std::string dataString_300 = vectorToString(sensorData.getDataVec300()); 
    std::string dataString_400 = vectorToString(sensorData.getDataVec400()); 
            
    content.set(info, infoString.c_str());
    content.set(condition, conString.c_str());
    content.set(data_200, dataString_200.c_str());
    content.set(data_300, dataString_300.c_str());
    content.set(data_400, dataString_400.c_str());

    // update_write.update_document_content = content.raw();


            // info is the collection id, countries is the document id in collection info.
            // updated path 2024/3/9 GROUP/MAC/DATE/TIME/ GROUP should be read from JSON but now it is hardcoded
            // TIME becomes a document, repeat time again for the fields for distinctive and dynamic fields
    
    // std::string documentPath = macAddressTest + "/" + today;
    // std::string RESTdocuPath = std::string(TARGET_GROUP.c_str()) + "/" + WiFi.macAddress().c_str();

            // Here's the critical part: specify the new field in the updateMask
    // std::string updateMask = std::string("d") + today + "_t" + currentTime; // This is "fields/<currentTime>"
    // update_write.update_masks = updateMask;
    // update_write.update_document_path = documentPath.c_str();
    // update_write.update_document_path = RESTdocuPath.c_str();

    std::string localPath = std::string(WiFi.macAddress().c_str()) + "/t_" + String(COUNT).c_str();
    
    vTaskDelay(10);
    localSave(localPath.c_str(), content);
    configIntMod("/FIREBASE_PATH", COUNT + 1);

    // writes.push_back(update_write);

    // int attempts = 0;
    // const int maxAttempts = 5; // Maximum number of retry attempts
    // bool success = false;
    // Firebase.ready();
    // while (!success && attempts < maxAttempts) {
    //     if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "", writes, "")) {
    //         Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    //         uploadState = Success;
    //         success = true;
    //         vTaskDelay(10);
    //     } else {
    //         Serial.println(fbdo.errorReason());
    //         uploadState = Failure;
    //         attempts++;
    //         vTaskDelay(5000);
    //     }
    // }

    // if (!success) {
    //     Serial.println("Failed to commit document after multiple attempts.");
    //     // Consider a fallback or notification mechanism
    // }


    updateUploadState(NULL);
        
    busy = false;
    uploadInProgress = false;
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("Minimum free stack for dataFF task: ");
    Serial.println(uxHighWaterMark);
    vTaskDelete(fsUploadTaskHandler);
}

//--- upload ---

// void dataFF(void *pvParameters){
//     Serial.println("Enter dataFF");
    
//     SensorDataFactory factory;
//     SensorData sensorData = factory.createSensorData();
    
//     uploadInProgress = true;

//     std::vector<struct firebase_firestore_document_write_t> writes;

//     struct firebase_firestore_document_write_t update_write;
//     update_write.type = firebase_firestore_document_write_type_update;
            
//     FirebaseJson content;
//     std::string macAddressTest = WiFi.macAddress().c_str();
            
//     // obtain date
//     struct tm timeinfo;
//     if (!getLocalTime(&timeinfo)) {
//         Serial.println("Failed to obtain time");
//         return;
//     }

//     char today[11]; // Buffer to hold the date string
//     strftime(today, sizeof(today), "%Y_%m_%d", &timeinfo); // Format: YYYY-MM-DD

//     char currentTime[9]; // Buffer to hold the time string
//     strftime(currentTime, sizeof(currentTime), "%H_%M_%S", &timeinfo); // Format: HH:MM:SS

//     std::string jsonKey = std::string("fields/d") + today + "_t" + currentTime;
//     std::string info = jsonKey + "/mapValue/fields/info/stringValue";
//     std::string condition = jsonKey + "/mapValue/fields/condition/stringValue";
//     std::string data_200 = jsonKey + "/mapValue/fields/data_200/stringValue";
//     std::string data_300 = jsonKey + "/mapValue/fields/data_300/stringValue";
//     std::string data_400 = jsonKey + "/mapValue/fields/data_400/stringValue";
            
//     std::string infoString = sensorData.getInfoString();
//     std::string conString = vectorToString(sensorData.getConVec()); 
//     std::string dataString_200 = vectorToString(sensorData.getDataVec200()); 
//     std::string dataString_300 = vectorToString(sensorData.getDataVec300()); 
//     std::string dataString_400 = vectorToString(sensorData.getDataVec400()); 
            
//     content.set(info, infoString.c_str());
//     content.set(condition, conString.c_str());
//     content.set(data_200, dataString_200.c_str());
//     content.set(data_300, dataString_300.c_str());
//     content.set(data_400, dataString_400.c_str());

//     update_write.update_document_content = content.raw();


//             // info is the collection id, countries is the document id in collection info.
//             // updated path 2024/3/9 GROUP/MAC/DATE/TIME/ GROUP should be read from JSON but now it is hardcoded
//             // TIME becomes a document, repeat time again for the fields for distinctive and dynamic fields
//     std::string documentPath = macAddressTest + "/" + today;
//     std::string RESTdocuPath = std::string(TARGET_GROUP.c_str()) + "/" + WiFi.macAddress().c_str();

//             // Here's the critical part: specify the new field in the updateMask
//     std::string updateMask = std::string("d") + today + "_t" + currentTime; // This is "fields/<currentTime>"
//     update_write.update_masks = updateMask;
//     // update_write.update_document_path = documentPath.c_str();
//     update_write.update_document_path = RESTdocuPath.c_str();

//     std::string localPath = std::string(WiFi.macAddress().c_str()) + "/d_"+ today + "_t" + currentTime;
    
//     vTaskDelay(10);
//     localSave(localPath.c_str(), content);

//     writes.push_back(update_write);

//     int attempts = 0;
//     const int maxAttempts = 5; // Maximum number of retry attempts
//     bool success = false;
//     Firebase.ready();
//     while (!success && attempts < maxAttempts) {
//         if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "", writes, "")) {
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//             uploadState = Success;
//             success = true;
//             vTaskDelay(10);
//         } else {
//             Serial.println(fbdo.errorReason());
//             uploadState = Failure;
//             attempts++;
//             vTaskDelay(5000);
//         }
//     }

//     if (!success) {
//         Serial.println("Failed to commit document after multiple attempts.");
//         // Consider a fallback or notification mechanism
//     }


//     updateUploadState(NULL);
        
//     busy = false;
//     uploadInProgress = false;
//     UBaseType_t uxHighWaterMark;
//     uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//     Serial.print("Minimum free stack for dataFF task: ");
//     Serial.println(uxHighWaterMark);
//     vTaskDelete(fsUploadTaskHandler);
// }


// void CreateTest(){


//         // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse/Create_Edit_Parse.ino
//         FirebaseJson content;

//         String documentPath = "test_collection/test_document" + String(count);

//         content.set("fields/myLatLng/geoPointValue/longitude", 23.678198);

//         count++;

//         Serial.print("Create a document... ");

//         if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());
    
    
// }

// void AppendMapTest()
// {
//         count++;
//         Serial.print("Commit a document (append map value in document)... ");
//         // The dyamic array of write object firebase_firestore_document_write_t.
//         std::vector<struct firebase_firestore_document_write_t> writes;
//         // A write object that will be written to the document.
//         struct firebase_firestore_document_write_t update_write;
//         update_write.type = firebase_firestore_document_write_type_update;
//         // Set the document content to write (transform)
//         FirebaseJson content;
//         String preDocPath = "test_collection/test_document";
//         String documentPath = "test_collection/test_document/sub_collection/sub_document";
//         content.set("fields/myMap"+ String(count) +"/mapValue/fields/key" + String(count) + "/stringValue", "value" + String(count));
//         // Set the update document content
//         update_write.update_document_content = content.raw();

//         update_write.update_masks = "myMap"+ String(count) +".key" + String(count);

//         // Set the update document path
//         update_write.update_document_path = documentPath.c_str();

//         // Add a write object to a write array.
//         writes.push_back(update_write);

//         if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of firebase_firestore_document_write_t */, "" /* transaction */))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());
    
// }

