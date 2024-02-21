// SensorData.cpp
#include <SensorData.h>

SensorData::SensorData(const std::string& info, const std::vector<int>& con, const std::vector<int>& data)
    : infoString(info), conVec(con), dataVec(data) {}

const std::string& SensorData::getInfoString() const {
    return infoString;
}

const std::vector<int>& SensorData::getConVec() const {
    return conVec;
}

const std::vector<int>& SensorData::getDataVec() const {
    return dataVec;
}

void SensorData::setInfoString(const std::string& info) {
    infoString = info;
}

void SensorData::setConVec(const std::vector<int>& con) {
    conVec = con;
}

void SensorData::setDataVec(const std::vector<int>& data) {
    dataVec = data;
}