// SensorDataFactory.h
#ifndef SENSOR_DATA_FACTORY_H
#define SENSOR_DATA_FACTORY_H

#include <SensorData.h>

class SensorDataFactory {
public:
    static SensorData createSensorData();
private:
    static void performSampling(std::vector<int>& conVec, std::vector<int>& dataVec);
    static int dummyData();
};

#endif // SENSOR_DATA_FACTORY_H
