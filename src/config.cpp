#include "config.h"

long freq1 = 433775000;
long freq2 = 439912500;
bool disableGPS = true;
bool disableLoRa = true;
bool disableDisplay = false;
bool disableSD = false;
bool disableSHT40 = false;
bool disableBLE = false;

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
