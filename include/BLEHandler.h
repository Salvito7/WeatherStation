#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLESecurity.h>
#include <BLE2902.h>
#include "sht40.h"
#include "power.h"
#include "errorHandler.h"

class BLEHandler {
public:
    BLEHandler();
    void setup();
    void updateSensorValues(float* temperature, float* humidity, double* voltage);
    void updateErrorCodes(const std::unordered_map<std::string, std::pair<std::string, int>>& errorCodes);
    void notifyDeepSleep(int time);

private:
    BLEServer* pServer;
    BLEService* pService;
    BLECharacteristic* pTemperatureCharacteristic;
    BLECharacteristic* pHumidityCharacteristic;
    BLECharacteristic* pVoltageCharacteristic;
    BLECharacteristic* pErrorCodesCharacteristic;
    BLECharacteristic* pDeepSleepCharacteristic;
};

#endif // BLE_HANDLER_H