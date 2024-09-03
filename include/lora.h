#ifndef NO_LORA

#ifndef LORA_H
#define LORA_H
#include <Arduino.h>

struct ReceivedLoRaPacket {
    String  text;
    int     rssi;
    float   snr;
    int     freqError;
};

class LoraType {
public:
    long  frequency;
    int   spreadingFactor;
    long  signalBandwidth;
    int   codingRate4;
    int   power;
};

namespace LoRa {

    void setFlag();
    void changeFreq();
    void setup();
    void sendNewPacket(const String& newPacket);
    void wakeRadio();
    ReceivedLoRaPacket receiveFromSleep();
    ReceivedLoRaPacket receivePacket();
    void sleepRadio();

}
#endif
#endif