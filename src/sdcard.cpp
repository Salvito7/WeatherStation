#ifndef NO_SD
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "display.h"
#include "config.h"
#include <logger.h>
#include "boards_pinout.h"

#ifdef TTGO_T_LORA32_V2_1
    int sck = 14;
    int miso = 2;
    int mosi = 15;
    int cs = 13;
#endif
#if defined(EXTERNAL_SD_MODULE) && !defined(TTGO_T_LORA32_V2_1) && !defined(TTGO_T_Beam_S3_SUPREME_V3)
    int sck = 27;
    int miso = 25;
    int mosi = 26;
    int cs = 14;
#endif

#ifdef TTGO_T_Beam_S3_SUPREME_V3
    int sck = 5;
    int miso = 19;
    int mosi = 27;
    int cs = 18;
#endif

extern logging::Logger logger;
extern String defaultFilename;
static String dataBuffer;
static bool noCard;
static String currentDir = "/";

namespace SDCARD {

    void setDefaultFilename(const char* filename) {
        defaultFilename = filename;
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Default filename set to: " + defaultFilename).c_str());
    }

    void appendDataToBuffer(const String& data) {
        dataBuffer += data;
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", ("Data appended to buffer: " + data).c_str());
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", ("Buffer size: " + String(dataBuffer.length())).c_str());
    }

    void setup() {
        #ifdef NO_SD
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "SD card is disabled");
            return;
        #else
            SPI.begin(sck, miso, mosi, cs);
            if (!SD.begin(cs)) {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Card Mount Failed");
                noCard = true;
                return;
            }
            uint8_t cardType = SD.cardType();

            if (cardType == CARD_NONE) {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "No SD card attached");
                noCard = true;
                return;
            }

            if(defaultFilename == "") {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "No default filename set!");
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Setting default filename to /default.txt");
                defaultFilename = "/default.txt";
            } 

            File defaultFile = SD.open(defaultFilename, FILE_READ, true);
            defaultFile.close();
        #endif
    }

    void loop() {
        if(!noCard) {
            unsigned long now = millis();
            static unsigned long lastMillis = 0;

            if ((now - lastMillis) >= 2000) {
                lastMillis = now;
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", ("Data buffer size: " + String(dataBuffer.length())).c_str());
                SPI.begin(sck, miso, mosi, cs);
                if(!SD.begin(cs)) {
                    if(SD.cardType() == CARD_NONE) {
                        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SD", "Waiting for SD card...");
                        noCard = true;
                        return;
                    } else {
                        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "SD card inserted");
                        noCard = false;
                        return;
                    }
                } else {
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "SD", "Waiting for SD card...");
                    noCard = true;
                    return;
                }
            }
            return;
        }

        unsigned long now = millis();
        static unsigned long lastMillis2 = 0;

        if (((now - lastMillis2) >= 2000)) {
            lastMillis2 = now;
            if(dataBuffer.length() < 48) {
                return;
            }
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", ("Data buffer size: " + String(dataBuffer.length())).c_str());

            File myFile = SD.open(defaultFilename, FILE_APPEND);
            if (!myFile) {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Failed to open file for appending");
                return;
            }
            if (myFile.print(dataBuffer)) {
                digitalWrite(LED_PIN, HIGH);
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Data appended to file");
                digitalWrite(LED_PIN, LOW);
            } else {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Append failed");
            }

            dataBuffer = "";
            myFile.close();
        }
    }

    File openFile(const char* path) {
        File myFile = SD.open(path, FILE_READ);
        if (myFile) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "File opened with success!");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Error opening file!");
        }
        return myFile;
    }

    void appendFile(const char *path, const char *message) {
        File file = SD.open(path, FILE_APPEND);
        if (!file) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Failed to open file for appending");
            return;
        }
        if (file.print(message)) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Message appended to file");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Append failed");
        }
        file.close();
    }

    String readFile(const char* path) {
        File myFile = SD.open(path, FILE_READ);
        String data = "";
        if (myFile) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Reading data from file: " + String(path)).c_str());
            while (myFile.available()) {
                data += (char)myFile.read();
            }
            myFile.close();
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Data read with success!");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Error reading data!");
        }
        return data;
    }

    void removeFile(const char *path) {
        if (SD.remove(path)) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("File removed: " + String(path)).c_str());
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Failed to remove file: " + String(path)).c_str());
        }
    }

    void listDir(const char *dirname, uint8_t levels) {
        File root = SD.open(dirname);
        if (!root) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Failed to open directory: " + String(dirname)).c_str());
            return;
        }
        if (!root.isDirectory()) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", (String(dirname) + " is not a directory").c_str());
            return;
        }

        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("DIR : " + String(file.name())).c_str());
                if (levels) {
                    listDir(file.name(), levels - 1);
                }
            } else {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("FILE: " + String(file.name()) + " SIZE: " + String(file.size())).c_str());
            }
            file = root.openNextFile();
        }
    }

    void changeDir(const char* dirname) {
        File dir = SD.open(dirname);
        if (dir && dir.isDirectory()) {
            currentDir = String(dirname);
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Changed directory to: " + currentDir).c_str());
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Failed to change directory to: " + String(dirname)).c_str());
        }
        dir.close();
    }

    String readFileInCurrentDir(const char* filename) {
        String fullPath = currentDir + "/" + String(filename);
        return readFile(fullPath.c_str());
    }

    void cardInfo() {
        uint8_t cardType = SD.cardType();
        if (cardType == CARD_NONE) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "No SD card attached");
            return;
        }

        if (cardType == CARD_MMC) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Card Type: MMC");
        } else if (cardType == CARD_SD) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Card Type: SD");
        } else if (cardType == CARD_SDHC) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Card Type: SDHC");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Card Type: UNKNOWN");
        }

        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("SD Card Size: " + String(cardSize) + "MB").c_str());

        #ifndef NO_DISPLAY
        // Display card info on screen if display is enabled
        #endif
    }
}
#endif