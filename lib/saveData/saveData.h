// saveData.h
#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <Arduino.h>
#include <lvgl.h>
#include <FirebaseJson.h>


extern volatile bool uploadInProgress;
extern volatile bool busy;

enum UploadState {
    NotStarted,
    Success,
    Failure
};
void updateUploadState(lv_timer_t *timer);
// void checkFile(char userID[]);
// void fsInit();
void localSave(String localPath, FirebaseJson sample);
void ensureDirectoriesExist(const String& path);
// void addDataJSON(FirebaseJsonArray& nestedData);
// void addArrayJSON(char userID[], const std::vector<std::vector<int>>& lists);
// void vectorDebug();
void firebaseSetup();

// void fireGetSample(const std::string& infoString, const std::vector<int>& conVec, const std::vector<int>& dataVec);
// void localRead();
void dataFF(void *pvParameters);
// void dataFF();
void firestoreUpload();
void fbKeepAlive();

void AppendMapTest();
void CreateTest();
void fbOTA();
// void dataFactoryTest();
// void fsCall();

#endif // SAVEDATA_H
