#include "config.h"
#include "lora.h"

long freq1 = 433775000;
long freq2 = 439912500;
bool disableGPS = true;
bool disableLoRa = true;
bool disableDisplay = false;
bool disableSD = false;
bool disableSensor = false;

String defaultFilename = "test.txt";

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
    }
    loraTypes[0].frequency = freq1;
    loraTypes[1].frequency = freq2;
}
