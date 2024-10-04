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
#include "time.h"

#ifndef NO_DISPLAY
  #include "display.h"
#endif

#ifndef NO_SD
  #include "sdcard.h"
#endif

#ifndef NO_LORA
  #include "lora.h"
#endif
#include <timeSetup.h>

#define MAX_ERROR_CODES 10
#define DEBUG
String version = "0.1beta";
logging::Logger logger;
CMDHandler commandHandler;
BLEHandler bleHandler;
ErrorHandler errorHandler;
TimeSetup timeSetup;

extern String defaultFilename;
extern bool disableLoRa;
extern bool disableDisplay;
extern bool disableSD;
extern bool disableSHT40;
extern bool disableBLE;
extern bool disableTimeSync;

void setup() { 
  Serial.begin(115200);

  #ifndef DEBUG
      logger.setDebugLevel(logging::LoggerLevel::LOGGER_LEVEL_INFO);
  #endif

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Starting up...");
  Serial.println("Version: " + String(version));
  delay(500);
  digitalWrite(LED_PIN, LOW);

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

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Time", ("Current time: " + timeSetup.getCurrentTime()).c_str());

  if(disableTimeSync) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "Time", "Time sync is disabled");
  } else {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Time", "Time setup");
    timeSetup.connectToWiFi();
    while(!timeSetup.synchronizeTime()) {}
    timeSetup.disconnectFromWiFi();
  }

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
  }

  #ifndef NO_SD
    if(!disableSD) {
      SDCARD::loop();
    }
  #else
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SD", "SD card is disabled");
  #endif
  
  commandHandler.processCommands();
  POWER::batteryManager();
  bleHandler.updateErrorCodes(errorHandler.getErrorCodes());
  delay(500);
}