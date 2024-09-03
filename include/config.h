#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#ifndef NO_LORA
    #include "lora.h"
    LoraType* getLoraTypes();
    LoraType* getLoraType(int index);
    void initLoRaTypes();
#endif
#endif