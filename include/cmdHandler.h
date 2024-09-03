#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

class CMDHandler {
public:
    void processCommands();
    void enterDeepSleep();

private:
    void listDirectories();
    void readSensorValues();
    void readDefaultFile();
    void displaySDCardInfo();
    void removeFile(const String& filename);
    
    void enableLoRa();
    void sendLoRaMessage(const String& message);
    void changeDirectory(const String& dirname);
    void appendToFile(const String& filename, const String& data);
    void readFileInCurrentDir(const String& filename);
    void showDirectoryOnDisplay();
    void showStatusOnDisplay();
    void scrollDirectoryUp();
    void scrollDirectoryDown();
};

#endif // COMMAND_HANDLER_H