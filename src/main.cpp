#include <SPI.h>
#include <Arduino.h>
#include <unordered_map>
#include <string>
#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include <logger.h>
#include <esp_bt.h>
#include <BluetoothSerial.h>
#include <SensirionI2cSht4x.h>
#include <WiFi.h>
#include <Wire.h>
#include "power.h"
#include "sht40.h"
#include "sdcard.h"
#include "config.h"
#include "boards_pinout.h"
#include "cmdHandler.h"
#include "errorHandler.h"
#include "BLEHandler.h"

#ifndef NO_DISPLAY
  #include "display.h"
#endif

#ifndef NO_SD
  #include "sdcard.h"
#endif

#ifndef NO_LORA
  #include "lora.h"
#endif

#define MAX_ERROR_CODES 10
#define DEBUG
float version = 1.0;
logging::Logger logger;
CMDHandler commandHandler;
BLEHandler bleHandler;
ErrorHandler errorHandler;

extern String defaultFilename;
extern bool disableLoRa;
extern bool disableDisplay;
extern bool disableSD;
extern bool disableSHT40;
extern bool disableBLE;

int adc_value = 0;
double voltage = 0.0;
double result = 0.0;

void setup() { 
  Serial.begin(115200);

  #ifndef DEBUG
      logger.setDebugLevel(logging::LoggerLevel::LOGGER_LEVEL_INFO);
  #endif

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("Starting up...");
  delay(500);
  digitalWrite(LED_PIN, HIGH);

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Power", "Power setup");  
  POWER::setup();
  #ifndef NO_DISPLAY
  if(disableDisplay) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "Display", "Display is disabled from config");
  } else {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "Display", "Display setup");
    setup_display();
    display_toggle(true);
    startupScreen(version);
  }
  #endif

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "WiFi", "Disabling WiFi");
  WiFi.mode(WIFI_OFF);

  if(disableBLE) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "BLE", "Disabling BLE");
    esp_bt_controller_disable();
  } else {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "BLE", "BLE setup");
    bleHandler.setup();
  }
  
  if(disableSHT40) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SHT40", "Disabling SHT40");
  } else {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SHT40", "SHT40 setup");
    SHT40::setup();
  }
  #ifndef NO_SD
    if(disableSD) {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SD", "Disabling SD");
    } else {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", "SD setup");
      SDCARD::setup();
      SDCARD::cardInfo();
    }
  #endif

  #ifndef NO_LORA
    if(disableLoRa) {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "LoRa", "Disabling LoRa");
    } else {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", "LoRa setup");
      initLoRaTypes(); 
    }
  #endif

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "PWR", "Lowering CPU frequency");
  POWER::lowerCpuFrequency();
}

void loop() {

#ifndef NO_LORA
  if(!disableLoRa) {
   // LoRa::loop();
  }
#endif

  #ifndef NO_DISPLAY
    if(!disableDisplay) {
      loop_display();
    }
  #endif

  if(!disableSHT40) {
    SHT40::loop();
    bleHandler.updateSensorValues(SHT40::getTemperature(), SHT40::getHumidity(), POWER::getBatteryVoltage());
  }

  #ifndef NO_SD
    if(!disableSD) {
    // logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", "SD loop");
      SDCARD::loop();
    }
  #endif
  commandHandler.processCommands();
  POWER::batteryManager();
  bleHandler.updateErrorCodes(errorHandler.getErrorCodes());
}