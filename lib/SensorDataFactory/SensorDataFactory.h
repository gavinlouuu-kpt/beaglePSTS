// SensorDataFactory.h
#ifndef SENSOR_DATA_FACTORY_H
#define SENSOR_DATA_FACTORY_H

#include <SensorData.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

extern volatile bool warmingInProgress;
extern volatile bool samplingInProgress;

class SensorDataFactory {
public:
    SensorData createSensorData();
    Adafruit_BME680 bme; // I2C
    void dataStream();

private:
    void performSampling(std::vector<float>& conVec, std::vector<uint32_t>& dataVec);
    static int dummyData();
    static int breath_check();
    #define SEALEVELPRESSURE_HPA (1013.25)
    bool bme_begin();
    void preSampling();
    void postSampling();
    // static void sample_collection();
};

#endif // SENSOR_DATA_FACTORY_H
