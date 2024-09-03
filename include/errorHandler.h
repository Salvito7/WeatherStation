#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <unordered_map>
#include <string>
#include <Arduino.h>

class ErrorHandler {
    public:
        void addErrorCode(const std::string& errorCode, const std::string& errorDescription);
        void displayErrorCodes() const;
        void clearAll();
        void clearErrorCode(const std::string& errorCode);
        std::unordered_map<std::string, std::pair<std::string, int>> getErrorCodes() const;
};

#endif
