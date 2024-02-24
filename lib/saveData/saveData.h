// saveData.h
#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <Arduino.h>
// #include <credential.h>

extern volatile bool uploadInProgress;

// void checkFile(char userID[]);
// void fsInit();
// void addDataJSON(char userID[], FirebaseJsonArray& nestedData);
// void addArrayJSON(char userID[], const std::vector<std::vector<int>>& lists);
// void vectorDebug();
void firebaseSetup();

// void fireGetSample(const std::string& infoString, const std::vector<int>& conVec, const std::vector<int>& dataVec);
// void localRead();
void dataFF(void *pvParameters);
// void dataFF();
void firestoreUpload();
void fbKeepAlive();
// void dataFactoryTest();
// void fsCall();

#endif // SAVEDATA_H
