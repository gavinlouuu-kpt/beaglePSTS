#include "saveData.h"
#include <WiFi.h>
#include <FirebaseJson.h>
#include <LittleFS.h>
#include <map>
#include <Arduino.h>
// #include <credential.h>
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

// void checkFile(char userID[]) {
//     const char* macAddress = WiFi.macAddress().c_str(); // Example MAC address
//     char filePath[64];
//     snprintf(filePath, sizeof(filePath), "/%s.json", macAddress);
//     Serial.println("filePath:");
//     Serial.println(filePath);
//     if (!LittleFS.exists(filePath)) {
//         Serial.println("Creating new file...");

//         FirebaseJson json;
//         char jsonKey[64];
//         const char* macAddressKey = WiFi.macAddress().c_str();
//         snprintf(jsonKey, sizeof(jsonKey), "%s/%s", macAddressKey, userID);
//         Serial.println("jsonKey:");
//         Serial.println(jsonKey);

//         // Set an empty object or a specific value as needed
//         json.set(jsonKey); //json.set(jsonKey,"") setting an empty string for demonstration

//         // Serialize JSON to std::string for writing
//         std::string jsonData;
//         json.toString(jsonData, true); // 'false' for compact serialization

//         // Write to file
//         File file = LittleFS.open(filePath, "w");
//         if (!file) {
//             Serial.println("Failed to open file for writing");
//             return;
//         }
//         file.print(jsonData.c_str());
//         file.close();

//         Serial.println("File created successfully.");
//     } else {
//         Serial.println("File already exists.");
//     }
// }

// void fsInit() {
//     const char* macAddress = WiFi.macAddress().c_str(); // Example MAC address
//     char filePath[64];
//     snprintf(filePath, sizeof(filePath), "/%s.json", macAddress);
//     Serial.println("filePath:");
//     Serial.println(filePath);
//     if (!LittleFS.exists(filePath)) {
//         Serial.println("Creating new file...");

//         FirebaseJson json;
//         char jsonKey[64];
//         const char* macAddressKey = WiFi.macAddress().c_str();
//         json.set(macAddressKey); //json.set(jsonKey,"") setting an empty string for demonstration

//         // Serialize JSON to std::string for writing
//         std::string jsonData;
//         json.toString(jsonData, true); // 'false' for compact serialization

//         // Write to file
//         File file = LittleFS.open(filePath, "w");
//         if (!file) {
//             Serial.println("Failed to open file for writing");
//             return;
//         }
//         file.print(jsonData.c_str());
//         file.close();

//         Serial.println("File created successfully.");
//     } else {
//         Serial.println("File already exists.");
//     }
// }



// void addDataJSON(char userID[], FirebaseJsonArray& nestedData) {
//     String macAddress = WiFi.macAddress();
//     String filePath = "/" + macAddress + ".json";  // Concatenate the MAC address with the file path

//     struct tm timeinfo;
//     if (!getLocalTime(&timeinfo)) {
//         Serial.println("Failed to obtain time");
//         return;
//     }

//     char today[11]; // Buffer to hold the date string
//     strftime(today, sizeof(today), "%Y-%m-%d", &timeinfo); // Format: YYYY-MM-DD

//     char currentTime[9]; // Buffer to hold the time string
//     strftime(currentTime, sizeof(currentTime), "%H:%M:%S", &timeinfo); // Format: HH:MM:SS

//     // Open the JSON file and read its contents
//     File file = LittleFS.open(filePath, "r");
//     if (!file) {
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     String fileContent;
//     while (file.available()) {
//         fileContent += char(file.read());
//     }
//     file.close();

//     // Load data into FirebaseJson object
//     FirebaseJson json;
//     json.setJsonData(fileContent);

//     // Check if today's date exists
//     FirebaseJsonData jsonData;
//     String path = String(macAddress) + "/" + userID + "/" + today;
//     if (!json.get(jsonData, path.c_str())) {
//         // Today's date does not exist, create it
//         FirebaseJson emptyJson;
//         String emptyJsonStr;
//         emptyJson.toString(emptyJsonStr, false); // Serialize empty JSON object to a string
//         json.set(path.c_str(), emptyJsonStr.c_str());
//     }

//     // Add or update data at the specific time
//     String timePath = String(currentTime);
//     path += "/" + timePath;

//     // Serialize the FirebaseJsonArray to a string
//     String nestedDataStr;
//     nestedData.toString(nestedDataStr);
    
//     json.set(path.c_str(), nestedDataStr.c_str());

//     // Serialize JSON back to String
//     String output;
//     json.toString(output, true); // Set to 'false' for non-pretty format

//     // Write updated JSON back to file
//     file = LittleFS.open(filePath, "w");
//     if (!file) {
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     file.print(output);
//     file.close();
// }


// void addDataJSON(char userID[], FirebaseJsonArray& nestedData) {
//     String macAddress = WiFi.macAddress();
//     String filePath = "/" + macAddress + ".json";  // Concatenate the MAC address with the file path

//     struct tm timeinfo;
//     if (!getLocalTime(&timeinfo)) {
//         Serial.println("Failed to obtain time");
//         return;
//     }

//     char today[11]; // Buffer to hold the date string
//     strftime(today, sizeof(today), "%Y-%m-%d", &timeinfo); // Format: YYYY-MM-DD

//     char currentTime[9]; // Buffer to hold the time string
//     strftime(currentTime, sizeof(currentTime), "%H:%M:%S", &timeinfo); // Format: HH:MM:SS

//     // Open the JSON file and read its contents
//     File file = LittleFS.open(filePath, "r");
//     if (!file) {
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     String fileContent;
//     while (file.available()) {
//         fileContent += char(file.read());
//     }
//     file.close();

//     // Load data into FirebaseJson object
//     FirebaseJson json;
//     json.setJsonData(fileContent);

//     // Check if today's date exists
//     FirebaseJsonData jsonData;
//     String path = String(macAddress) + "/" + today;
//     if (!json.get(jsonData, path.c_str())) {
//         // Today's date does not exist, create it
//         FirebaseJson emptyJson;
//         String emptyJsonStr;
//         emptyJson.toString(emptyJsonStr, false); // Serialize empty JSON object to a string
//         json.set(path.c_str(), emptyJsonStr.c_str());
//     }

//     // Add or update data at the specific time
//     String timePath = String(currentTime);
//     path += "/" + timePath;

//     // Serialize the FirebaseJsonArray to a string
//     String nestedDataStr;
//     nestedData.toString(nestedDataStr);
    
//     json.set(path.c_str(), nestedDataStr.c_str());

//     // Serialize JSON back to String
//     String output;
//     json.toString(output, true); // Set to 'false' for non-pretty format

//     // Write updated JSON back to file
//     file = LittleFS.open(filePath, "w");
//     if (!file) {
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     file.print(output);
//     file.close();
// }


void populateJsonArray(FirebaseJsonArray& jsonArray, int* list, int length) {
    for (int i = 0; i < length; i++) {
        jsonArray.add(list[i]);
    }
}


// void vectorDebug(){
//     for (const auto& list : globalLists) {
//         for (const auto& num : list) {
//         Serial.print(num);
//         Serial.print(" ");
//         }
//     Serial.println();
//     }
// }

// void arrayTestInit(){
//     std::vector<int> list_1 = {1, 2, 3, 4, 5};
//     std::vector<int> list_2 = {5, 4, 3, 2, 1};
//     std::vector<int> list_3 = {5, 4, 3, 2, 1, 1,1};
//     globalLists.clear();
//     globalLists.push_back(list_1);
//     globalLists.push_back(list_2);
//     globalLists.push_back(list_3);
// }

// void addArrayJSON(char userID[], const std::vector<std::vector<int>>& lists) {
//     // Serial.println("Before addArray:"); //debug
//     // vectorDebug(); //debug
//     arrayTestInit();

//     FirebaseJsonArray nestedData;
//     for (const auto& list : lists) {
//         FirebaseJsonArray innerArray;
//         for (int num : list) {
//             innerArray.add(num);
//         }
//         nestedData.add(innerArray);
//         // Serial.println("addArray Loop:"); //debug
//         // vectorDebug(); //debug
//     }
//     addDataJSON(userID, nestedData);
// }

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
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

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


// void localRead(){
//     std::string macAddress = WiFi.macAddress().c_str();
//     std::string filePath = "/" + macAddress + ".json";  // Concatenate the MAC address with the file path

//     // Open the file as before
//     File file = LittleFS.open(filePath.c_str(), "r");
//     if (!file) {
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     // Determine the size of the file
//     size_t fileSize = file.size();

//     // Reserve space in the string to avoid multiple reallocations
//     std::string fileContent;
//     fileContent.reserve(fileSize);

//     // Create a buffer for reading
//     const size_t bufferSize = 512; // Adjust the buffer size depending on available memory
//     char buffer[bufferSize];

//     // Read the file in blocks
//     while (file.available()) {
//         int bytesRead = file.readBytes(buffer, bufferSize);
//         fileContent.append(buffer, bytesRead);
//     }

//     file.close();

//     // chunk read
//     FirebaseJson json;
//     json.setJsonData(fileContent);
//     // json.toString(fileContent, true);
//     json.toString(Serial, true);
//     // individual get 
//     Serial.println();

//     std::string macAddressKey = WiFi.macAddress().c_str();
//     std::string jsonKey = macAddressKey + "/" + userID;

//     Serial.println("jsonKey:");
//     Serial.println(jsonKey.c_str());

//     FirebaseJsonData result; // object that keeps the deserializing result
//     json.get(result, jsonKey.c_str());
//     Serial.println("get result:");
//     Serial.println(result.to<String>().c_str());

    

// }

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
              16384,
              NULL,
              1,
              &fsUploadTaskHandler);
}


volatile bool uploadInProgress = false;

// void dataFF(void *pvParameters){
//     Serial.println("Enter dataFF");
    
//     SensorDataFactory factory;
//     SensorData sensorData = factory.createSensorData();
    
//     if (Firebase.ready() && (millis() - dataMillis > 30000 || dataMillis == 0))
//     {
//         uploadInProgress = true;
//         dataMillis = millis();
//             // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse/Create_Edit_Parse.ino
//             FirebaseJson content;
//             std::string macAddressTest = WiFi.macAddress().c_str();
//             // obtain date
//             struct tm timeinfo;
//             if (!getLocalTime(&timeinfo)) {
//                 Serial.println("Failed to obtain time");
//                 return;
//             }

//             char today[11]; // Buffer to hold the date string
//             strftime(today, sizeof(today), "%Y-%m-%d", &timeinfo); // Format: YYYY-MM-DD

//             char currentTime[9]; // Buffer to hold the time string
//             strftime(currentTime, sizeof(currentTime), "%H_%M_%S", &timeinfo); // Format: HH:MM:SS

//             std::string jsonKey = std::string("fields/t") + currentTime;
//             std::string info = jsonKey + "/mapValue/fields/info/stringValue";
//             std::string condition = jsonKey + "/mapValue/fields/condition/stringValue";
//             std::string data = jsonKey + "/mapValue/fields/data/stringValue";
            
//             std::string infoString = sensorData.getInfoString();
//             std::string conString = vectorToString(sensorData.getConVec()); 
//             std::string dataString = vectorToString(sensorData.getDataVec()); 
            
//             content.set(info, infoString.c_str());
//             content.set(condition, conString.c_str());
//             content.set(data, dataString.c_str());
//             // info is the collection id, countries is the document id in collection info.
//             std::string documentPath = macAddressTest + "/" + today;

//             // Here's the critical part: specify the new field in the updateMask
//             std::string updateMask = std::string("t") + currentTime; // This is "fields/<currentTime>"
            
//             if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), updateMask /* updateMask */))
//                 Serial.println("ok");
                
//             else
//                 Serial.println(fbdo.errorReason());
//         }
//     uploadInProgress = false;
//     UBaseType_t uxHighWaterMark;
//     uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//     Serial.print("Minimum free stack for dataFF task: ");
//     Serial.println(uxHighWaterMark);
//     vTaskDelete(fsUploadTaskHandler);
// }
UploadState uploadState = NotStarted;

void dataFF(void *pvParameters){
    Serial.println("Enter dataFF");
    
    SensorDataFactory factory;
    SensorData sensorData = factory.createSensorData();
    
    uploadInProgress = true;
        // dataMillis = millis();

            // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse/Create_Edit_Parse.ino
            // The dyamic array of write object firebase_firestore_document_write_t.
    std::vector<struct firebase_firestore_document_write_t> writes;
            // A write object that will be written to the document.
    struct firebase_firestore_document_write_t update_write;
    update_write.type = firebase_firestore_document_write_type_update;
            // Set the document content to write (transform)
            
    FirebaseJson content;
    std::string macAddressTest = WiFi.macAddress().c_str();
            // obtain date
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }

    char today[11]; // Buffer to hold the date string
    strftime(today, sizeof(today), "%Y-%m-%d", &timeinfo); // Format: YYYY-MM-DD

    char currentTime[9]; // Buffer to hold the time string
    strftime(currentTime, sizeof(currentTime), "%H_%M_%S", &timeinfo); // Format: HH:MM:SS

    std::string jsonKey = std::string("fields/t") + currentTime;
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

    update_write.update_document_content = content.raw();


            // info is the collection id, countries is the document id in collection info.
    std::string documentPath = macAddressTest + "/" + today;

            // Here's the critical part: specify the new field in the updateMask
    std::string updateMask = std::string("t") + currentTime; // This is "fields/<currentTime>"
    update_write.update_masks = updateMask;
    update_write.update_document_path = documentPath.c_str();

    writes.push_back(update_write);

    if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of firebase_firestore_document_write_t */, "" /* transaction */)){
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        uploadState = Success;
    }   
    else{
        Serial.println(fbdo.errorReason());
        uploadState = Failure;
    }
    updateUploadState(NULL);
        
            // if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), updateMask /* updateMask */))
            //     Serial.println("ok");
                
            // else
            //     Serial.println(fbdo.errorReason());
    busy = false;
    uploadInProgress = false;
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("Minimum free stack for dataFF task: ");
    Serial.println(uxHighWaterMark);
    vTaskDelete(fsUploadTaskHandler);
}




void AppendMapTest()
{
        count++;
        Serial.print("Commit a document (append map value in document)... ");
        // The dyamic array of write object firebase_firestore_document_write_t.
        std::vector<struct firebase_firestore_document_write_t> writes;
        // A write object that will be written to the document.
        struct firebase_firestore_document_write_t update_write;
        update_write.type = firebase_firestore_document_write_type_update;
        // Set the document content to write (transform)
        FirebaseJson content;
        String documentPath = "test_collection/test_document";
        content.set("fields/myMap"+ String(count) +"/mapValue/fields/key" + String(count) + "/stringValue", "value" + String(count));
        // Set the update document content
        update_write.update_document_content = content.raw();

        update_write.update_masks = "myMap"+ String(count) +".key" + String(count);

        // Set the update document path
        update_write.update_document_path = documentPath.c_str();

        // Add a write object to a write array.
        writes.push_back(update_write);

        if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of firebase_firestore_document_write_t */, "" /* transaction */))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
    
}
