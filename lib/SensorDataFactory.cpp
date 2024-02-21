// SensorDataFactory.cpp
#include <SensorDataFactory.h>

// Example implementation; needs to be filled with actual logic
void SensorDataFactory::performSampling(std::vector<int>& conVec, std::vector<int>& dataVec) {
    // Dummy implementation - Replace this with actual sampling code
    conVec = {25, 35, 75, 77, 10004};
    dataVec = {5000, 5002, 5003, 5004, 5005};
}

SensorData SensorDataFactory::createSensorData() {
    std::string infoString = "Temp,eTemp,RH,eRH,dur";
    std::vector<int> conVec;
    std::vector<int> dataVec;
    performSampling(conVec, dataVec);
    return SensorData(infoString, conVec, dataVec);
}
