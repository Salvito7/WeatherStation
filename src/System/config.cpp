#include "config.h"

long freq1 = 433775000;
long freq2 = 439912500;
bool disableGPS = true;
bool disableLoRa = true;
bool disableDisplay = false;
bool disableSD = false;
bool disableSHT40 = false;
bool disableBLE = false;

// WiFi credentials
String ssid = "ABC";
String password = "123";

// Set NTP server and time zone
bool disableTimeStamps = false;
bool disableTimeSync = true;
String ntpServer1 = "pool.ntp.org";
String ntpServer2 = "time.nist.gov";
long gmtOffsetInSeconds = 7200; //GMT +2

String defaultFilename = "/test.txt";

//TODO add more LoRa types and make this whole thing configuarble with a .json file from SPIFFS or SD card

#ifndef NO_LORA
    static LoraType loraTypes[2];
    int loraIndexSize = 2;

    LoraType* getLoraTypes() {
        return loraTypes;
    }

    LoraType* getLoraType(uint8_t loraIndex) {
        return &loraTypes[loraIndex];
    }

    void initLoRaTypes() {
        for (int i = 0; i < loraIndexSize; i++) {
            loraTypes[i] = { 0, 12, 125000, 5, 20 };
            //LoraType { frequency, spreadingFactor, signalBandwidth, codingRate4, power }        
        }
        loraTypes[0].frequency = freq1;
        loraTypes[1].frequency = freq2;
    }
#endif
