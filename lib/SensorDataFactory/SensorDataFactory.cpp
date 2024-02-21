// SensorDataFactory.cpp
#include <Arduino.h>
#include "SensorDataFactory.h"
#include <chrono>
#include <thread>

// // Example implementation; needs to be filled with actual logic
// void SensorDataFactory::performSampling(std::vector<int>& conVec, std::vector<int>& dataVec) {
//     // Dummy implementation - Replace this with actual sampling code
//     conVec = {25, 35, 75, 77, 10004};

//     dataVec.clear(); // Make sure it's empty before filling
//     // Start from 5000, end at 5999 to get exactly 1000 values
//     for(int i = 5000; i < 6000; ++i) {
//         dataVec.push_back(i);
//     }
//     // dataVec = {5000, 5002, 5003, 5004, 5005};
// }

int SensorDataFactory::dummyData(){
    //create random number between 1000 and 2000
    int random = 1000 + (rand() % 1000);
    return random;
}

void SensorDataFactory::performSampling(std::vector<int>& conVec, std::vector<int>& dataVec) {
    using namespace std::chrono;
// record initial condition
    for(int i = 0; i < 5; ++i){
        conVec.push_back(dummyData());
    }

//create a loop for a minute where dummyData is called every 60ms 
    auto start = steady_clock::now();
    auto end = start + seconds(60);
    dataVec.clear(); // Make sure it's empty before filling
    while(steady_clock::now() < end){
        dataVec.push_back(dummyData());
        Serial.print(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }

// record ending condition
    for(int i = 0; i < 5; ++i){
        conVec.push_back(dummyData());
    }

}

SensorData SensorDataFactory::createSensorData() {
    std::string infoString = "T_s, RH_s, Pa_s, T_e, RH_e,Pa_e, t_ms";
    std::vector<int> conVec;
    std::vector<int> dataVec;
    performSampling(conVec, dataVec);
    return SensorData(infoString, conVec, dataVec);
}
