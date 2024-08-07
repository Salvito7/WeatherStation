#include "config.h"
#include "lora.h"

long freq1 = 433775000;
long freq2 = 439912500;

extern LoraType loraTypes[2];
uint8_t loraIndex;
int loraIndexSize = 2;

LoraType* getLoraTypes() {
    return loraTypes;
}

LoraType getLoraType() {
    return loraTypes[loraIndex];
}

void initLoRaTypes() {
    for (int i = 0; i < loraIndexSize; i++) {
        loraTypes[i] = { 0, 12, 125000, 5, 20 };
    }
    loraTypes[0].frequency = freq1;
    loraTypes[1].frequency = freq2;
}
