// SensorData.h
#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <string>
#include <vector>

struct SensorData {
private:
    std::string infoString;
    std::vector<int> conVec;
    std::vector<int> dataVec;

public:
    // Constructor
    SensorData(const std::string& info, const std::vector<int>& con, const std::vector<int>& data);

    // Getters
    const std::string& getInfoString() const;
    const std::vector<int>& getConVec() const;
    const std::vector<int>& getDataVec() const;

    // Setters
    void setInfoString(const std::string& info);
    void setConVec(const std::vector<int>& con);
    void setDataVec(const std::vector<int>& data);
};

#endif // SENSOR_DATA_H
