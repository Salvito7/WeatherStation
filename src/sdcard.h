#ifndef SDCARD_H_
#define SDCARD_H_

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
        //Dont forget to close the file with closeFile( file );
        File openFile(const char* filename);
        void closeFile(File file);
        void removeFile(const char* filename);
        void listDir(const char* dirname);
};

#endif