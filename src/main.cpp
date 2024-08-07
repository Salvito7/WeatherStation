#include <Arduino.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include "power.h"
#include <WiFi.h>
#include <esp_bt.h>
#include <BluetoothSerial.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include "display.h"
#include "sht40.h"
#include "lora.h"

long freq1 = 433775000;
long freq2 = 439912500;

SensirionI2cSht4x sensor;
extern BluetoothSerial  SerialBT;

void setup() { 
  Serial.begin(115200);
  POWER::setup();
  setup_display();
  WiFi.mode(WIFI_OFF);
  esp_bt_controller_disable();
  setup_sensor();
  display_toggle(true);

  #ifdef LORA_ENABLED
    initLoRaTypes(); 
  #endif

  startupScreen("1.0");
  POWER::lowerCpuFrequency();
}

void loop() {
  POWER::batteryManager();
}