#include "saveData.h"
#include <WiFi.h>
// #include <FirebaseJson.h>
#include <LittleFS.h>
#include <map>
#include <Arduino.h>
#include <credential.h>
#include <stdio.h>
#include <string>
#include <beagleCLI.h>

extern std::map<String, std::function<void()>> commandMap;

void ESPinfo(){
    uint32_t flash_size = ESP.getFlashChipSize();
    Serial.print("Flash size: ");
    Serial.print(flash_size);
    Serial.println(" bytes");
}



String processCommand(const String& receivedCommand) {
    auto it = commandMap.find(receivedCommand);
    if (it != commandMap.end()) {
        it->second(); // execute the command
        Serial.println();
        Serial.println("Command executed");
        return "tc"; // task complete
    } else {
        Serial.println();
        Serial.println("Unknown command");
        return "uc"; // unknown command
    }
}

String readSerialInput() {
    String input = "";
    while (!Serial.available()) {
        delay(10); // small delay to allow buffer to fill
    }
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            break;
        }
        input += c;
        delay(10); // small delay to allow buffer to fill
    }
    return input;
}

void printFileContent() {
    Serial.println("Enter the file name to open:");
    String fileName = readSerialInput();
    Serial.println("Opening file: " + fileName);

    File file = LittleFS.open("/" + fileName, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("Contents of the file:");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}


void listFilesInDirectory() {
    File root = LittleFS.open("/");

    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    Serial.println("List of files:");
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("DIR : ");
            Serial.println(file.name());
        } else {
            Serial.print("FILE: ");
            Serial.println(file.name());
        }
        file = root.openNextFile();
    }
}


bool deleteAllFilesInLittleFS() {
    return deleteAllFilesInDirectory("/");
}

bool deleteAllFilesInDirectory(const char *dirPath) {
    File dir = LittleFS.open(dirPath);
    if (!dir || !dir.isDirectory()) {
        Serial.println(String("Failed to open directory: ") + dirPath);
        return false;
    }

    File file = dir.openNextFile();
    while (file) {
        String filePath;
        if (String(dirPath) == "/") {
            filePath = "/" + String(file.name());
        } else {
            filePath = String(dirPath) + "/" + file.name();
        }

        if (file.isDirectory()) {
            if (!deleteAllFilesInDirectory(filePath.c_str())) {
                Serial.println(String("Failed to delete directory: ") + filePath);
                return false;
            }
            LittleFS.rmdir(filePath.c_str());
        } else {
            file.close(); // Close the file if it's open
            if (!LittleFS.remove(filePath.c_str())) {
                Serial.println(String("Failed to remove file: ") + filePath);
                return false;
            }
        }
        file = dir.openNextFile();
    }

    return true;
}

void cmdSetup() {
    // commandMap["init"] = [&]() { fsInit(); };
    commandMap["deleteAll"] = []() { deleteAllFilesInLittleFS();};
    commandMap["ls"] = []() { listFilesInDirectory(); };
    commandMap["open"] = []() { printFileContent(); };
    // commandMap["arraytest"] = []() { addArrayJSON(userID,globalLists); };
    // commandMap["fireGetSample"] = []() { fireGetSample(); };
    // commandMap["dataFF"] = []() { dataFF(); };
    commandMap["info"] = []() { ESPinfo(); };
    // commandMap["localRead"] = []() { localRead(); };
    // commandMap["dataFactory"] = []() { dataFactoryTest(); };
    commandMap["help"] = [&]() {
    Serial.println("Available commands:");
    for (const auto& command : commandMap) {
        Serial.println(command.first);
    }
};
}

void beagleCLI() {
    if (Serial.available() > 0) {
        String receivedCommand = Serial.readStringUntil('\n');
        receivedCommand.trim();
        Serial.println("Received command: " + receivedCommand);
        processCommand(receivedCommand);            
    }
}

void CLI_Task(void *pvParameters) {
    while (true) {
        beagleCLI();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void CLI_Call(){
    xTaskCreate(CLI_Task,
              "CLI_Task",
              2048,
              NULL,
              1,
              NULL);
}