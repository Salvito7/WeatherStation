#include "errorHandler.h"
#include <iostream>

std::unordered_map<std::string, std::pair<std::string, int>> errorCodes;

void ErrorHandler::addErrorCode(const std::string& errorCode, const std::string& errorDescription) {
    if(errorCodes.count(errorCode) > 0) {
        errorCodes[errorCode].second++;
        return;
    }
    errorCodes[errorCode] = std::make_pair(errorDescription, 1);
}

void ErrorHandler::displayErrorCodes() const {
    std::cout << "Error Codes:" << std::endl;
    for (const auto& pair : errorCodes) {
        std::cout << pair.first << ": " << pair.second.first << ", " << pair.second.second << std::endl;
    }
}

void ErrorHandler::clearAll() {
    errorCodes.clear();
}

void ErrorHandler::clearErrorCode(const std::string& errorCode) {
    errorCodes.erase(errorCode);
}

std::unordered_map<std::string, std::pair<std::string, int>> ErrorHandler::getErrorCodes() const {
    return errorCodes;
}