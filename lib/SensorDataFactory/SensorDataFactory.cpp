// SensorDataFactory.cpp
#include <Arduino.h>
#include "SensorDataFactory.h"
#include <chrono>
#include <vector>
#include <thread>
#include <pinConfig.h>


#include <vector>
#include <chrono>
#include <algorithm> // For std::copy

int SensorDataFactory::breath_check(){
    auto start_time = std::chrono::steady_clock::now(); // Record start time for timeout check
    auto previoustime = std::chrono::steady_clock::now();
    std::vector<int> Sensor_arr; // Vector to store sensor readings dynamically.

    while (true) {
        int sensorValue = dummyData(); // Placeholder for actual sensor reading logic.

        // Add new sensor value, ensure vector does not exceed 100 elements.
        if (Sensor_arr.size() < 100) {
            Sensor_arr.push_back(sensorValue);
        } else {
            // Shift vector contents, discarding the oldest reading.
            std::copy(Sensor_arr.begin() + 1, Sensor_arr.end(), Sensor_arr.begin());
            Sensor_arr.back() = sensorValue;
        }

        // Every 500 milliseconds, check if the change in sensor values indicates a breath.
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - previoustime).count() > 500) {
            if (Sensor_arr.size() >= 100 && Sensor_arr.back() - Sensor_arr.front() > 200) {
                Serial.print("Baseline: ");
                Serial.println(Sensor_arr.front());
                return Sensor_arr.front(); // Return the baseline value indicating a breath detected.
            }
            previoustime = now; // Reset the timer for the next interval check.
        }

        // Check for timeout (60 seconds) to return an error if no breath is detected.
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 60) {
            Serial.println("Error: No breath detected within 60 seconds.");
            return -1; // Return an error code.
        }
    }
}






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
