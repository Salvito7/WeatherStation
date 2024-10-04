#include "timeSetup.h"
#include <WiFi.h>
#include "time.h"
#include "logger.h" 
#include "config.h"

extern logging::Logger logger;
extern String ssid;
extern String password;
extern String ntpServer;
extern String ntpServer2;
extern long gmtOffsetInSeconds;

void TimeSetup::connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "TimeSetup", "Connected to WiFi!");
}

void TimeSetup::disconnectFromWiFi() {
    WiFi.disconnect();
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "TimeSetup", "Disconnected from WiFi!");
    WiFi.mode(WIFI_OFF);
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "WiFi", "Disabling WiFi");
    
}

bool TimeSetup::synchronizeTime() {
    configTime(gmtOffsetInSeconds, 0, ntpServer.c_str(), ntpServer2.c_str());
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "TimeSetup", "Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) { // 8 * 3600 * 2 = 16 hours
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "TimeSetup", "Time synchronized!");
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "TimeSetup", asctime(&timeinfo));
    //Serial.println(asctime(&timeinfo));
    return true;
}

String TimeSetup::getCurrentTime() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    return asctime(&timeinfo);
}