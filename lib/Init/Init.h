#ifndef INIT_h
#define INIT_h

// void FactoryReset();

String readConfigValue(const char *path, const char *jsonPath);
void configInit();
void writeFile(const char *path, const char *message);

extern int pumpSpeed;
extern String FIREBASE_PROJECT_ID;
extern String USER_EMAIL;
extern String USER_PASSWORD;
extern String API_KEY;
extern String DATABASE_URL;
extern long gmtOffset_sec;
extern int daylightOffset_sec;
extern String STORAGE_BUCKET_ID;


#endif