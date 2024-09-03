#include "BLEHandler.h"

//TODO maybe add a way to change the BLE name from config
//TODO maybe switch to NimBLE for better performance and lower memory usage

//TODO BLE not reporting correct values

BLEHandler::BLEHandler() : pServer(nullptr), pService(nullptr), pTemperatureCharacteristic(nullptr), pHumidityCharacteristic(nullptr), pVoltageCharacteristic(nullptr), pErrorCodesCharacteristic(nullptr), pDeepSleepCharacteristic(nullptr) {}

void BLEHandler::setup() {
    BLEDevice::init("BLE_Server");

    esp_log_level_set("BLEHandler", ESP_LOG_DEBUG);
    
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    pSecurity->setCapability(ESP_IO_CAP_OUT);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setKeySize(16);
    pSecurity->setStaticPIN(123456);

    pServer = BLEDevice::createServer();
    pService = pServer->createService(BLEUUID((uint16_t)0x180A));

    pTemperatureCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0x2A6E),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pTemperatureCharacteristic->addDescriptor(new BLE2902());

    pHumidityCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0x2A6F),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pHumidityCharacteristic->addDescriptor(new BLE2902());

    pVoltageCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0x2A19),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pVoltageCharacteristic->addDescriptor(new BLE2902());

    pErrorCodesCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0x2A3D),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pErrorCodesCharacteristic->addDescriptor(new BLE2902());

    pDeepSleepCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0x2A3E),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pDeepSleepCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

void BLEHandler::updateSensorValues(float* temperature, float* humidity, double* voltage) {
    pTemperatureCharacteristic->setValue(*temperature);
    pTemperatureCharacteristic->notify();

    pHumidityCharacteristic->setValue(*humidity);
    pHumidityCharacteristic->notify();

    pVoltageCharacteristic->setValue(*voltage);
    pVoltageCharacteristic->notify();
}

void BLEHandler::updateErrorCodes(const std::unordered_map<std::string, std::pair<std::string, int>>& errorCodes) {
    static unsigned long lastMillis = 0;
    if ((millis() - lastMillis) >= 5000) {
        lastMillis = millis();
        std::string errorCodesStr;
        for (const auto& pair : errorCodes) {
            errorCodesStr += pair.first + ": " + pair.second.first + " " + std::to_string(pair.second.second) + "\n";
        }
        pErrorCodesCharacteristic->setValue(errorCodesStr);
        pErrorCodesCharacteristic->notify();
    }
}

void BLEHandler::notifyDeepSleep(int time) {
    pDeepSleepCharacteristic->setValue("Entering deep sleep mode for " + std::to_string(time) + " seconds");
    pDeepSleepCharacteristic;
}