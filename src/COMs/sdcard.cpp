#ifndef NO_SD
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "display.h"
#include "config.h"
#include <logger.h>
#include "boards_pinout.h"
#include "errorHandler.h"
#include "vfs_api.cpp"

#ifdef TTGO_T_LORA32_V2_1
    int sck = 14;
    int miso = 2;
    int mosi = 15;
    int cs = 13;
#endif
#if defined(EXTERNAL_SD_MODULE) && !defined(TTGO_T_LORA32_V2_1) && !defined(TTGO_T_Beam_S3_SUPREME_V3)
    int sck = 26;
    int miso = 25;
    int mosi = 27;
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
extern bool disableSD;
extern ErrorHandler errorHandler;
extern bool disableTimeSync;

static String dataBuffer;
static String currentDir = "/";
static File logFile;

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
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "No SD card");
                errorHandler.addErrorCode("SD", "No SD card");
                disableSD = true;
            }
            uint8_t cardType = SD.cardType();

            if(defaultFilename == "") {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "No default filename set!");
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Setting default filename to /default.txt");
                
            } else if (defaultFilename.indexOf("%date%") != -1 || defaultFilename.indexOf("%time%") != -1) {
                // Get current date and time
                time_t now = time(nullptr);
                struct tm *timeinfo = localtime(&now);

                // Format the date and time as strings
                char date[11];
                char time[9];
                strftime(date, sizeof(date), "%Y%m%d", timeinfo);
                strftime(time, sizeof(time), "%H%M%S", timeinfo);
                defaultFilename.replace("%date%", date);
                defaultFilename.replace("%time%", time);
                
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Setting default filename to: " + defaultFilename).c_str());
            } 

            // Create default file if it doesn't exist    
            logFile = SD.open(defaultFilename, FILE_APPEND, true);

            //add header 
            if (!disableTimeSync) {
                logFile.print("");
                logFile.print("Date,Time,Temperature,Humidity\n");
            } else {
                logFile.print("");
                logFile.print("Temperature,Humidity\n");
            }
        #endif
    }

    void loop() {
        if(!disableSD) {
            unsigned long now = millis();
            static unsigned long lastMillis = 0;

            if (((now - lastMillis) >= 5000)) {
                lastMillis = now;
                if(dataBuffer.length() < 48) { //check if the buffer has 12 (lenght of a single data line from the sensor) x 4 bytes to avoid cutting off data
                    return;
                }
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SD", ("Data buffer size: " + String(dataBuffer.length())).c_str());

                if (!logFile) {
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Failed to open file for appending. Reopening file");
                    //errorHandler.addErrorCode("SD", "Open file for appending failed");
                    logFile = SD.open(defaultFilename, FILE_APPEND);
                    //TODO add some way of detecting if the error is due to the filesystem not being mounted => remount
                    return;
                }
                if (logFile.print(dataBuffer)) {
                    digitalWrite(LED_PIN, HIGH);
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Data appended to file");
                    digitalWrite(LED_PIN, LOW);
                } else {
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Append failed");
                    errorHandler.addErrorCode("SD", "Append failed");
                }

                dataBuffer = "";
            }
        }
    }

    void closeDefaultFile() {
        logFile.close();
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "File closed");
    }

    File openFile(const char* path) {
        File myFile = SD.open(path, FILE_READ);
        if (myFile) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "File opened with success!");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Error opening file!");
            errorHandler.addErrorCode("SD", "Error opening file");
        }
        return myFile;
    }

    void appendFile(const char *path, const char *message) {
        File file = SD.open(path, FILE_APPEND);
        if (!file) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Failed to open file for appending");
            errorHandler.addErrorCode("SD", "Failed to open file for appending");
            return;
        }
        if (file.print(message)) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Message appended to file");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", "Append failed");
            errorHandler.addErrorCode("SD", "Append failed");
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
            errorHandler.addErrorCode("SD", "Error reading data");
        }
        return data;
    }

    void removeFile(const char *path) {
        if (SD.remove(path)) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("File removed: " + String(path)).c_str());
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Failed to remove file: " + String(path)).c_str());
            errorHandler.addErrorCode("SD", "Failed to remove file");
        }
    }

    void listDirTerminal(const char *dirname, uint8_t levels) {
        File root = SD.open(dirname);
        if (!root) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Failed to open directory: " + String(dirname)).c_str());
            errorHandler.addErrorCode("SD", "Failed to open directory");
            return;
        }
        if (!root.isDirectory()) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", (String(dirname) + " is not a directory").c_str());
            errorHandler.addErrorCode("SD", "Not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("DIR : " + String(file.name())).c_str());
                if (levels) {
                    listDirTerminal(file.name(), levels - 1);
                }
            } else {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("FILE: " + String(file.name()) + " SIZE: " + String(file.size())).c_str());
            }
            file = root.openNextFile();
        }
    }
    

    String listDir(const char* dirname, int scrollIndex) {
      File root = SD.open(dirname);
      if (!root) {
          return "Failed to open directory";
      }
      if (!root.isDirectory()) {
          return "Not a directory";
      }

      String dirList = "";
      int index = 0;
      File file = root.openNextFile();
      while (file) {
          if (index >= scrollIndex && index < scrollIndex + 5) {
              dirList += file.name();
              dirList += "\n";
          }
          file = root.openNextFile();
          index++;
      }
      return dirList;
    }

    String getCurrentDir() {
        return currentDir;
    }

    void changeDir(const char* dirname) {
        File dir = SD.open(dirname);
        if (dir && dir.isDirectory()) {
            currentDir = String(dirname);
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Changed directory to: " + currentDir).c_str());
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Failed to change directory to: " + String(dirname)).c_str());
            errorHandler.addErrorCode("SD", "Failed to change directory");
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
            errorHandler.addErrorCode("SD", "No SD card attached");
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

        uint64_t cardSize = SD.cardSize() / (1024 * 1024); //in MB
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("SD Card Size: " + String(cardSize) + "MB").c_str());
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Used space: " + String(SD.usedBytes()) + " bytes").c_str());

        #ifndef NO_DISPLAY
        // Display card info on screen if display is enabled
        #endif
    }
}
#endif