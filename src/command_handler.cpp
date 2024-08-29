#include "command_handler.h"
#include "sht40.h"
#include "sdcard.h"
#include "power.h"
#include "lora.h"
#include <logger.h>

extern logging::Logger logger;
extern String defaultFilename;
extern bool disableLoRa;

void CommandHandler::setup() {
    Serial.begin(115200);
}

void CommandHandler::processCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == "help") {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CmdHandler", "Available commands:");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "list - List directories");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "readsensor - Read sensor values");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "psens - Pause sensor readings");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "rsens - Resume sensor readings");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "rdefault - Read default file");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "sdinfo - Display SD card info");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "sleep - Enter deep sleep for 5 seconds");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "enablelora - Enable LoRa");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "disablelora - Disable LoRa");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "remove <filename> - Remove file");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "sendlora <message> - Send LoRa message");
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "cd <dirname> - Change directory"); // New command
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CmdHandler", "append <filename> <data> - Append data to file"); // New command
            return;
        
        } else if (command == "list") {
            listDirectories();
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
        } else if (command == "enablelora") {
            enableLoRa();
        } else if (command == "disablelora") {
            disableLoRa = true;
            POWER::deactivateLoRa();
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "LoRa disabled");
        } else {
            if (command.startsWith("remove ")) {
                String filename = command.substring(7);
                removeFile(filename);
            } else if (command.startsWith("sendlora ")) {
                String message = command.substring(10);
                sendLoRaMessage(message);
            } else if (command.startsWith("cd ")) { // New command
                String dirname = command.substring(3);
                changeDirectory(dirname);
            } else if (command.startsWith("append ")) { // New command
                int firstSpace = command.indexOf(' ', 7);
                if (firstSpace != -1) {
                    String filename = command.substring(7, firstSpace);
                    String data = command.substring(firstSpace + 1);
                    appendToFile(filename, data);
                } else {
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "CMDHandler", "Invalid append command format");
                }
            } else if (command.startsWith("readfile ")) { // New command
                String filename = command.substring(9);
                readFileInCurrentDir(filename);
            } else {
                logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "CMDHandler", "Unknown command");
            }
        }
    }
}

void CommandHandler::listDirectories() {
    SDCARD::listDir("/", 0);
}

void CommandHandler::readSensorValues() {
    SHT40::loop();
}

void CommandHandler::readDefaultFile() {
    String data = SDCARD::readFile(defaultFilename.c_str());
    Serial.println(data);
}

void CommandHandler::displaySDCardInfo() {
    SDCARD::cardInfo();
}

void CommandHandler::removeFile(const String& filename) {
    SDCARD::removeFile(filename.c_str());
}

void CommandHandler::enterDeepSleep() {
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "Entering deep sleep for 5 seconds");
    esp_sleep_enable_timer_wakeup(5000000);
    esp_deep_sleep_start();
}

void CommandHandler::enableLoRa() {
    #ifndef NO_LORA
        disableLoRa = false;
        POWER::activateLoRa();
        LoRa::setup();
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "LoRa enabled");
    #else
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "LoRa is disabled by hardware"); //not really disabled by hardware, but by the NO_LORA flag in platformio.ini 
    #endif
}

void CommandHandler::sendLoRaMessage(const String& message) {
    #ifndef NO_LORA
        LoRa::sendNewPacket(message);
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "CMDHandler", "LoRa message sent");
    #else
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "CMDHandler", "LoRa is disabled");
    #endif
}

void CommandHandler::changeDirectory(const String& dirname) {
    SDCARD::changeDir(dirname.c_str());
}

void CommandHandler::appendToFile(const String& filename, const String& data) {
    SDCARD::appendFile(filename.c_str(), data.c_str());
}

void readFileInCurrentDir(const String& filename) {
    SDCARD::readTextFile(filename.c_str());
}