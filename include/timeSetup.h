#ifndef TIME_H
#define TIME_H

#include <Arduino.h>


class TimeSetup {
public:
    void connectToWiFi();
    bool synchronizeTime();
    void disconnectFromWiFi();
    String getCurrentTime(); // Replace 'String' with 'std::string'
};

#endif // TIME_H