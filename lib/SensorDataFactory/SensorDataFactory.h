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
    static int breath_check();
    static void sample_collection();
};

#endif // SENSOR_DATA_FACTORY_H
