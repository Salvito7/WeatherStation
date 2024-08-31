#ifndef NO_SD
#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD.h>

namespace SDCARD {
    void setup();
    void loop();
    void cardInfo();
    void setDefaultFilename(const char* filename);
    String readFile(const char* filename);
    void appendFile(const char* filename, const char* data);
    void appendDataToBuffer(const String& data);
    File openFile(const char* filename);
    void removeFile(const char* filename);
    void listDirTerminal(const char *dirname, uint8_t levels);
    String listDir(const char *dirname, int scrollIndex);
    void changeDir(const char* dirname);
    String getCurrentDir();
    String readFileInCurrentDir(const char* filename);
}

#endif
#endif