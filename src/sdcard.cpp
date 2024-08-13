#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "display.h"
#include "config.h"
#include <logger.h>
    
#ifdef TTGO_T_LORA32_V2_1_GPS 
    int sck = 14;
    int miso = 2;
    int mosi = 15;
    int cs = 13;
#else
    #ifdef EXTERNAL_SD_MODULE
        int sck = -1;
        int miso = -1;
        int mosi = -1;
        int cs = -1;
    #endif
#endif


extern logging::Logger logger;
extern String defaultFilename;
static String dataBuffer;
File defaultFile;

namespace SDCARD {

  void setDefaultFilename(const char* filename) {
      defaultFilename = filename;
  }

  void appendDataToBuffer(const String& data) {
      dataBuffer += data;
  }

  void setup() {
    SPI.begin(sck, miso, mosi, cs);
    if (!SD.begin(cs)) {
      Serial.println("Card Mount Failed");
      return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
    }
  }

  void loop() {
      if(dataBuffer.length() <= 1) {
          return;
      }
      
      File myFile = SD.open(defaultFilename, FILE_APPEND);
      // check if the SD card is available to write data without blocking
      // and if the dataBuffered data is enough for the full chunk size
      unsigned int chunkSize = myFile.availableForWrite();
      if (chunkSize && dataBuffer.length() >= chunkSize) {
          // write to file and blink LED
          digitalWrite(LED_BUILTIN, HIGH);
          myFile.write((const uint8_t*)dataBuffer.c_str(), chunkSize);
          digitalWrite(LED_BUILTIN, LOW);
          // remove written data from dataBuffer
          dataBuffer.remove(0, chunkSize);
      }
      myFile.close();
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

  void closeFile(File file) {
      file.close();
  }

  void appendFile(const char *path, const char *message) {
      File file = SD.open(path, FILE_APPEND);
  if (!file) {
      Serial.println("Failed to open file for appending");
      return;
    }
    if (file.print(message)) {
      Serial.println("Message appended");
    } else {
      Serial.println("Append failed");
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
          logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("File " + String(path) + " removed with success!").c_str());
      } else {
          logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "SD", ("Error removing file " + String(path)).c_str());
      }
  }

  void listDir(const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);

    File root = SD.open(dirname);
    if (!root) {
      Serial.println("Failed to open directory");
      return;
    }
    if (!root.isDirectory()) {
      Serial.println("Not a directory");
      return;
    }

    File file = root.openNextFile();
    while (file) {
      if (file.isDirectory()) {
        Serial.print("  DIR : ");
        Serial.println(file.name());
        if (levels) {
          listDir(file.path(), levels - 1);
        }
      } else {
        Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("  SIZE: ");
        Serial.println(file.size());
      }
      file = root.openNextFile();
    }
  }

  void cardInfo() {

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "MMC");
    } else if (cardType == CARD_SD) {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "SD");
    } else if (cardType == CARD_SDHC) {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "SDHC");
    } else {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("SD Card Size: " + String(cardSize) + "MB").c_str());
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", "Volume Info:");
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Total space: " + String(SD.totalBytes() / (1024 * 1024)) + "MB").c_str());
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "SD", ("Used space: " + String(SD.usedBytes() / (1024 * 1024)) + "MB").c_str());

    show_display("SD Card Info", "Card type: ", ""+SD.cardType(), "Volume size (MB): ", String((SD.totalBytes() / (1024 * 1024)) - (SD.usedBytes() / (1024 * 1024))), "", 5000);
  }

}



