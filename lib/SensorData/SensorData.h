// SensorData.h
#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <string>
#include <vector>

struct SensorData {
private:
    std::string infoString;
    std::vector<float> conVec;
    std::vector<uint32_t> dataVec;

public:
    // Constructor
    SensorData(const std::string& info, const std::vector<float>& con, const std::vector<uint32_t>& data);

    // Getters
    const std::string& getInfoString() const;
    const std::vector<float>& getConVec() const;
    const std::vector<uint32_t>& getDataVec() const;

    // Setters
    void setInfoString(const std::string& info);
    void setConVec(const std::vector<float>& con);
    void setDataVec(const std::vector<uint32_t>& data);
};

#endif // SENSOR_DATA_H
