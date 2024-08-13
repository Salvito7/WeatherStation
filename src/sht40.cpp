#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include "sdcard.h"
#include <logger.h>

#define SHT40_SDA 33
#define SHT40_SCL 32

SensirionI2cSht4x sensor;
TwoWire I2C_SHT40 = TwoWire(0);

static char errorMessage[64];
static int16_t error;
extern logging::Logger logger;

namespace SHT40 {

    void setup() {  
        I2C_SHT40.begin(SHT40_SDA, SHT40_SCL); 

        sensor.begin(I2C_SHT40, SHT40_I2C_ADDR_44);
        sensor.softReset();
        delay(10);
        uint32_t serialNumber = 0;
        error = sensor.serialNumber(serialNumber);
        if (error != 0) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SHT40", "Sensor error in serialNumber(), CHECK WIRING!");
            errorToString(error, errorMessage, sizeof errorMessage);
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SHT40", errorMessage);
            return;
        }
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SHT40", "Sensor serial number: ");
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SHT40", String(serialNumber).c_str());
    }

    void loop() {

        unsigned long lastMillis = 0;
        unsigned long now = millis();

        if ((now - lastMillis) >= 1000) {

            float temperature = 0;
            float humidity = 0;
            error = sensor.measureHighPrecision(temperature, humidity);
            if (error != 0) {
                Serial.print("Error trying to execute measure(): ");
                errorToString(error, errorMessage, sizeof errorMessage);
                Serial.println(errorMessage);
                return;
            }
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.println(" °C");
            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println(" %");
            SDCARD::appendDataToBuffer((String(temperature) + "," + String(humidity) + "\n").c_str());
            lastMillis = now;
        }
    }
}