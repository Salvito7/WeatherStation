#include <Arduino.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include <logger.h>
#include <esp_bt.h>
#include <BluetoothSerial.h>
#include <SensirionI2cSht4x.h>
#include <WiFi.h>
#include <Wire.h>
#include "power.h"
#include "display.h"
#include "sht40.h"
#include "lora.h"
#include "sdcard.h"
#include "config.h"

float version = 1.0;
logging::Logger logger;
//#define DEBUG

extern String defaultFilename;
extern bool disableLoRa;
extern bool disableDisplay;
extern bool disableSD;
extern bool disableSensor;

void setup() { 
  Serial.begin(115200);

  #ifndef DEBUG
      logger.setDebugLevel(logging::LoggerLevel::LOGGER_LEVEL_INFO);
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  POWER::setup();
  
  if(disableDisplay) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Display", "Display is disabled");
  } else {
    setup_display();
    display_toggle(true);
  }

  WiFi.mode(WIFI_OFF);
  esp_bt_controller_disable();

  if(disableSensor) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Sensor", "Sensor is disabled");
  } else {
    SHT40::setup();
  }

  if(disableSD) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "SD is disabled");
  } else {
    SDCARD::setup();
    SDCARD::cardInfo();
    SDCARD::setDefaultFilename("test.txt");
  }

  if(disableLoRa) {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", "LoRa is disabled");
  } else {
    initLoRaTypes(); 
  }

  startupScreen(version);
  POWER::lowerCpuFrequency();
}

void loop() {
  SHT40::loop();
  SDCARD::loop();	
  POWER::batteryManager();
}