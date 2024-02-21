// credential.cpp
#include "credential.h"
#include <vector>
#include <string>

char userID[] = "1155089131";
char WIFI_SSID[] = "KenNew";
char WIFI_PASSWORD[] = "85223982660";
const char ntpServer[] = "pool.ntp.org";
long gmtOffset_sec = 28800;  // Local time offset
int daylightOffset_sec = 0;  // Daylight offset



std::vector<std::vector<int>> globalLists;
// std::string 