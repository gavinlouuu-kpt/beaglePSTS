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

int pumpSpeed;

#include <FirebaseJson.h>

#include <FirebaseJson.h>

// Function to read a generic value from a JSON file and return it as a String
String readConfigValue(const char *path, const char *jsonPath) {
  File file = LittleFS.open(path, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return ""; // Return an empty string to indicate failure
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

  // Extract the value
  if (!json.get(jsonData, jsonPath)) {
    Serial.print("Failed to read ");
    Serial.print(jsonPath);
    Serial.println(" from JSON");
    return ""; // Return an empty string to indicate failure
  }

  // Return the value as a String
  return jsonData.stringValue;
}



void configInit(){
  String pumpSpeedStr = readConfigValue("/config.json","/pump_speed");
  pumpSpeed = pumpSpeedStr.toInt();
  
  //other use cases
  // String isEnabledStr = readConfigValue("/config.json", "/is_enabled");
  // bool isEnabled = isEnabledStr.equalsIgnoreCase("true"); // Simple way to convert string to boolean
  // Serial.print("Is Enabled: ");
  // Serial.println(isEnabled ? "True" : "False");
  // String temperatureStr = readConfigValue("/config.json", "/temperature");
  // float temperature = temperatureStr.toFloat(); // Convert string to float
  // Serial.print("Temperature: ");
  // Serial.println(temperature);
}

// // Factory Reset
void FactoryReset(){
  // read file at "/factory/preset.json" retrieve pumpSpeed and write the speed to "/config.json"
  // a factory reset is essentially copying preset.json to config.json
  // if the file exists, delete it
  if (LittleFS.exists("/config.json")) {
    LittleFS.remove("/config.json");
  }
  // copy the file
  if (LittleFS.exists("/factory/preset.json")) {
    File file = LittleFS.open("/factory/preset.json", FILE_READ);
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }
    String fileContent;
    while (file.available()) {
      fileContent += char(file.read());
    }
    file.close();
    writeFile("/config.json", fileContent.c_str());
  }
}


// Initialize Device
