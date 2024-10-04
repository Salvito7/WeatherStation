#include "cmdHandler.h"
#include "sht40.h"
#include "sdcard.h"
#include "power.h"
#include "lora.h"
#include <logger.h>
#include "errorHandler.h"


#ifndef NO_DISPLAY
#include "display.h"
#endif

extern logging::Logger logger;
extern String defaultFilename;
extern bool disableLoRa;
extern bool disableDisplay;
extern bool disableSD;
extern bool disableSHT40;
extern bool disableBLE;
extern ErrorHandler errorHandler;

static int directoryScrollIndex = 0;

void CMDHandler::processCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == "help") {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "Available commands:");

            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "Sensor commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "readsensor - Read sensor values");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "psens - Pause sensor readings");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "rsens - Resume sensor readings");

            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "File commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "rdefault - Read default file");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "sdinfo - Display SD card info");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "remove <filename> - Remove file");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "append <filename> <data> - Append data to file");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "readfile <filename> - Read file");

            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "Directory commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "list - List directories");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "cd <dirname> - Change directory");
            #ifndef NO_DISPLAY
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "up - Scroll up");
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "down - Scroll down");
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "exit - Exit to status screen");
            #endif

            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "System commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "sleep - Enter deep sleep for 5 seconds");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "shutdown - Enter deep sleep for 4 hours");

            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "LoRa commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "sendlora <message> - Send LoRa message");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "enablelora - Enable LoRa");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "disablelora - Disable LoRa");

            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "Error handling commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "showerrors - Show error codes");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "clearerrors - Clear error codes");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "clearerror <errorcode> - Clear specific error code");
            return;
        
        } else if (command == "list") {
            SDCARD::listDirTerminal(SDCARD::getCurrentDir().c_str(), 0);
            showDirectoryOnDisplay();
        } else if (command == "readsensor") {
            readSensorValues();
        } else if (command == "psens") {
            SHT40::pauseReadings();
        } else if (command == "rsens") {
            SHT40::resumeReadings();
        } else if (command == "rdefault") {
            readDefaultFile();
        } else if (command == "sdinfo") {
            displaySDCardInfo();
        } else if (command == "sleep") {
            enterDeepSleep();
        } else if (command == "shutdown") {
            POWER::shutdown();
        } else if (command == "enablelora") {
            enableLoRa();
        } else if (command == "disablelora") {
            disableLoRa = true;
        } else if (command == "showerrors") {
            errorHandler.displayErrorCodes();  
        } else if (command == "up") {
            scrollDirectoryUp();
        } else if (command == "down") {
            scrollDirectoryDown();
        } else if (command == "exit") {
            showStatusOnDisplay();
        } else if (command == "clearerrors") {
            errorHandler.clearAll();
        } else if (command.startsWith("clearerror ")) {
            String errorCode = command.substring(11);
            errorHandler.clearErrorCode(errorCode.c_str());
        } else {
            if (command.startsWith("remove ")) {
                String filename = command.substring(7);
                removeFile(filename);
            } else if (command.startsWith("sendlora ")) {
                String message = command.substring(10);
                sendLoRaMessage(message);
            } else if (command.startsWith("cd ")) { 
                String dirname = command.substring(3);
                changeDirectory(dirname);
            } else if (command.startsWith("append ")) {
                int firstSpace = command.indexOf(' ', 7);
                if (firstSpace != -1) {
                    String filename = command.substring(7, firstSpace);
                    String data = command.substring(firstSpace + 1);
                    appendToFile(filename, data);
                } else {
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "CMDHandler", "Invalid append command format");
                }
            } else if (command.startsWith("readfile ")) {
                String filename = command.substring(9);
                SDCARD::readFileInCurrentDir(filename.c_str());
            } else {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "CMDHandler", "Unknown command");
            }
        }
    }
}

void CMDHandler::listDirectories() {
    SDCARD::listDir("/", 0);
}

void CMDHandler::readSensorValues() {
    SHT40::loop();
}

void CMDHandler::readDefaultFile() {
    String data = SDCARD::readFile(defaultFilename.c_str());
    Serial.println(data);
}

void CMDHandler::displaySDCardInfo() {
    SDCARD::cardInfo();
}

void CMDHandler::removeFile(const String& filename) {
    SDCARD::removeFile(filename.c_str());
}

void CMDHandler::enterDeepSleep() {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "Entering deep sleep for 5 seconds");
    esp_sleep_enable_timer_wakeup(30000000);
    delay(500);
    esp_deep_sleep_start();
}

void CMDHandler::enableLoRa() {
    #ifndef NO_LORA
        disableLoRa = false;
        POWER::activateLoRa();
        LoRa::setup();
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "LoRa enabled");
    #else
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "LoRa is disabled by hardware"); //not really disabled by hardware, but by the NO_LORA flag in platformio.ini 
    #endif
}

void CMDHandler::sendLoRaMessage(const String& message) {
    #ifndef NO_LORA
        LoRa::sendNewPacket(message);
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "LoRa message sent");
    #else
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "LoRa is disabled");
    #endif
}

void CMDHandler::changeDirectory(const String& dirname) {
    SDCARD::changeDir(dirname.c_str());
}

void CMDHandler::appendToFile(const String& filename, const String& data) {
    SDCARD::appendFile(filename.c_str(), data.c_str());
}

void CMDHandler::showDirectoryOnDisplay() {
    #ifndef NO_DISPLAY
    String currentDir = SDCARD::getCurrentDir();
    String dirList = SDCARD::listDir(currentDir.c_str(), directoryScrollIndex);
    show_display("Directory: " + currentDir, dirList, "", 0);
    #endif
}

void CMDHandler::showStatusOnDisplay() {
    #ifndef NO_DISPLAY
    float temperature = SHT40::getTemperature();
    float humidity = SHT40::getHumidity();
    float voltage = POWER::getBatteryVoltage();
    String status = "Temp: " + String(temperature) + "C\n" +
                    "Humidity: " + String(humidity) + "%\n" +
                    "Voltage: " + String(voltage) + "V";
    show_display("Status", status, "", 0);
    #endif
}

void CMDHandler::scrollDirectoryUp() {
    if (directoryScrollIndex > 0) {
        directoryScrollIndex--;
        showDirectoryOnDisplay();
    }
}

void CMDHandler::scrollDirectoryDown() {
    directoryScrollIndex++;
    showDirectoryOnDisplay();
}