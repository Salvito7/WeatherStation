#include "config.h"

long freq1 = 433775000;
long freq2 = 439912500;
bool disableGPS = true;
bool disableLoRa = true;
bool disableDisplay = false;
bool disableSD = false;
bool disableSHT40 = false;

String defaultFilename = "/test.txt";

#ifndef NO_LORA
    static LoraType loraTypes[2];
    int loraIndexSize = 2;

    LoraType* getLoraTypes() {
        return loraTypes;
    }

    LoraType* getLoraType(uint8_t loraIndex) {
        return &loraTypes[loraIndex];
    }

    //TODO add more LoRa types that are read from .json file from SPIFFS or SD card
    void initLoRaTypes() {
        for (int i = 0; i < loraIndexSize; i++) {
            loraTypes[i] = { 0, 12, 125000, 5, 20 };
            //LoraType { frequency, spreadingFactor, signalBandwidth, codingRate4, power }        
        }
        loraTypes[0].frequency = freq1;
        loraTypes[1].frequency = freq2;
    }
#endif
