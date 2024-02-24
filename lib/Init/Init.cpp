#include <Arduino.h>
#include <FirebaseJson.h>
#include <LittleFS.h>
#include <FS.h>
#include <lvgl.h>

// // LittleFS system
void writeFile(const char *path, const char *message) {
  File file = LittleFS.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

int readPumpSpeed(const char *path) {
  File file = LittleFS.open(path, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return -1;
  }

  // Read the content of the file into a string
  String fileContent;
  while (file.available()) {
    fileContent += char(file.read());
  }
  file.close();

  // Use FirebaseJson to parse the JSON string
  FirebaseJson json;
  json.setJsonData(fileContent);

  // Create a FirebaseJsonData object to store the result
  FirebaseJsonData jsonData;

  // Extract the pump_speed value
  if (!json.get(jsonData, "/pump_speed")) {
    Serial.println("Failed to read pump_speed from JSON");
    return -1; // or handle the error as appropriate
  }

  // Assuming pump_speed is an integer
  int pumpSpeed = jsonData.intValue;
  Serial.println("Pump speed:");
  Serial.print(pumpSpeed);

  return pumpSpeed;
}


// // Factory Reset
// void FactoryReset(){
//     if (LittleFS.begin()) {
//         Serial.println("LittleFS mounted successfully");
//         writeFile("/config.json", "{}");
//     } else {
//         Serial.println("An error occurred during LittleFS mounting...");
//     }
    
// }


// Initialize Device
