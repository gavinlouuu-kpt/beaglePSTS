// SensorData.cpp
#include <SensorData.h>

SensorData::SensorData(const std::string& info, const std::vector<float>& con, const std::vector<uint32_t>& data)
    : infoString(info), conVec(con), dataVec(data) {}

const std::string& SensorData::getInfoString() const {
    return infoString;
}

const std::vector<float>& SensorData::getConVec() const {
    return conVec;
}

const std::vector<uint32_t>& SensorData::getDataVec() const {
    return dataVec;
}

void SensorData::setInfoString(const std::string& info) {
    infoString = info;
}

void SensorData::setConVec(const std::vector<float>& con) {
    conVec = con;
}

void SensorData::setDataVec(const std::vector<uint32_t>& data) {
    dataVec = data;
}