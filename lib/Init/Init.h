#ifndef INIT_h
#define INIT_h

// void FactoryReset();

String readConfigValue(const char *path, const char *jsonPath);
void configInit();
void writeFile(const char *path, const char *message);

extern int pumpSpeed;

#endif