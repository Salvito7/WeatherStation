#ifndef SHT40_H
#define SHT40_H

#include <Arduino.h>

namespace SHT40 {

    void setup();
    void loop();
    void pauseReadings();
    void resumeReadings();
    float getTemperature();
    float getHumidity();

}

#endif // SHT40_H