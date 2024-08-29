#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

class CommandHandler {
public:
    void setup();
    void processCommands();

private:
    void listDirectories();
    void readSensorValues();
    void readDefaultFile();
    void displaySDCardInfo();
    void removeFile(const String& filename);
    void enterDeepSleep();
    void enableLoRa();
    void sendLoRaMessage(const String& message);
    void changeDirectory(const String& dirname); 
    void appendToFile(const String& filename, const String& data); 
    void readFileInCurrentDir(const String& filename);
};

#endif // COMMAND_HANDLER_H