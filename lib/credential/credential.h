
// credential.h
#include <vector>
#include <string>

#ifndef CREDENTIAL_H
#define CREDENTIAL_H

extern char userID[];

// Other variables
extern char WIFI_SSID[];
extern char WIFI_PASSWORD[];
extern const char ntpServer[];
// extern long gmtOffset_sec;
// extern int daylightOffset_sec;

/* 2. Define the API Key */
// #define API_KEY "AIzaSyBX06Su-f2xPLp7ThCoOj-Q0Z4-JVJKcIM"
/* 3. Define the project ID */
// #define FIREBASE_PROJECT_ID "beaglepsts-88c3f"
/* 4. Define the user Email and password that alreadey registerd or added in your project */

// #define USER_EMAIL "gavinlou@kingsphase.page"
// #define USER_PASSWORD "PSTS2024"

// #define DATABASE_URL "https://beaglepsts-88c3f-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Forward declaration of required classes
class FirebaseJsonArray;

// // Declare global variables with extern
// extern std::vector<int*> globalLists;

extern std::vector<std::vector<int>> globalLists;




#endif // CREDENTIAL_H
