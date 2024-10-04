#include <Arduino.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include <logger.h>
#include "config.h"
#include "errorHandler.h"
#include "BLEHandler.h"
#ifndef NO_SD
    #include "sdcard.h"
#endif

#define SHT40_SDA 32
#define SHT40_SCL 33

SensirionI2cSht4x sensor;
TwoWire I2C_SHT40 = TwoWire(0);
extern ErrorHandler errorHandler;
extern BLEHandler bleHandler;
extern logging::Logger logger;
extern bool disableSHT40;
extern bool disableSD;
extern bool disableBLE;
extern bool disableTimeStamps;

static char errorMessage[64];
static float temperature = 0.0;
static float humidity = 0.0;

namespace SHT40 {
    
    unsigned long lastMillis = 0;

    void setup() {  
        I2C_SHT40.begin(SHT40_SDA, SHT40_SCL); 
        sensor.begin(I2C_SHT40, SHT40_I2C_ADDR_44);
        sensor.softReset();
        delay(10);
        uint32_t serialNumber = 0;
        int16_t error = sensor.serialNumber(serialNumber);
        if (error != 0) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SHT40", "Error requesting serialNumber(), CHECK WIRING!");
            errorToString(error, errorMessage, sizeof errorMessage);
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SHT40", errorMessage);
            disableSHT40 = true;
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SHT40", "Disabling SHT40");
            errorHandler.addErrorCode("SHT40", "Error requesting serialNumber()");
            return;
        }
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SHT40", "Sensor serial number: ");
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SHT40", String(serialNumber).c_str());
        #ifndef NO_SD
            if(!disableSD) {
            }   
        #endif
    }

    void loop() {
        if (disableSHT40) {
            return;
        }
        unsigned long now = millis();

        if ((now - lastMillis) >= 5000) {
            lastMillis = now;
            int16_t error = sensor.measureMediumPrecision(temperature, humidity);
            if (error != 0) {
                Serial.print("Error trying to execute measure(): ");
                errorToString(error, errorMessage, sizeof errorMessage);
                Serial.println(errorMessage);
                errorHandler.addErrorCode("SHT40", "Error trying to execute measure()");
                temperature = 0.0;
                humidity = 0.0;
                return;
            }
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SHT40", ("Temperature: " + String(temperature) + " C").c_str());
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SHT40", ("Humidity: " + String(humidity) + " %").c_str());
            if(!disableBLE) bleHandler.updateSensorValues();
            //Serial.println("SHT40: " + String(SHT40::getTemperature()) + " C, " + String(SHT40::getHumidity()) + " %");
            #ifndef NO_SD
                if(!disableSD) {
                    SDCARD::appendDataToBuffer((String(temperature) + "," + String(humidity) + "\n").c_str());
                }   
            #endif
        }
    }

    void pauseReadings() {
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SHT40", "Pausing SHT40 readings");
        disableSHT40 = true;
    }

    void resumeReadings() {
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SHT40", "Resuming SHT40 readings");
        disableSHT40 = false;
    }

    float getTemperature() {
        return temperature;
    }

    float getHumidity() {
        return humidity;
    }
}