#ifndef BEAGLECLI_H
#define BEAGLECLI_H

#include <Arduino.h>

void cmdSetup();
void beagleCLI();
String processCommand(const String& receivedCommand); 
void printFileContent();
void printHexFileContent();
// void listFilesInDirectory();
void listFilesInDirectory(const String& directoryPath = "/");
bool deleteAllFilesInLittleFS();
bool deleteAllFilesInDirectory(const char *dirPath);
void CLI_Call();
void CLI_Task(void *pvParameters);

#endif // SAVEDATA_H