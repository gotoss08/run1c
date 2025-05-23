#include "error_handler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Windows.h>

// Static member definition
ErrorHandler::ErrorCallback ErrorHandler::errorCallback;

void ErrorHandler::showError(ErrorType type, const std::string& details) {
    std::string message = formatErrorMessage(type, details);
    std::cerr << "[ERROR] " << message << std::endl;
    
    if (errorCallback) {
        errorCallback(type, details);
    }
}

void ErrorHandler::showErrorWithDialog(ErrorType type, const std::string& details) {
    std::string message = formatErrorMessage(type, details);
    std::cerr << "[ERROR] " << message << std::endl;
    
    // Show Windows message box
    MessageBoxA(nullptr, message.c_str(), "RUN1C Error", MB_OK | MB_ICONERROR);
    
    if (errorCallback) {
        errorCallback(type, details);
    }
}

bool ErrorHandler::validatePath(const std::string& path) {
    if (path.empty()) {
        logInfo("Path validation failed: path is empty");
        return false;
    }
    
    try {
        bool exists = std::filesystem::exists(path);
        if (!exists) {
            logInfo("Path validation failed: path does not exist: " + path);
        }
        return exists;
    } catch (const std::exception& e) {
        logError(ErrorType::InvalidPath, "Path validation failed: " + std::string(e.what()) + " for path: " + path);
        return false;
    }
}

bool ErrorHandler::validate1CPath(const std::string& path) {
    if (!validatePath(path)) {
        return false;
    }
    
    try {
        std::filesystem::path filepath(path);
        return filepath.filename() == "1cestart.exe";
    } catch (const std::exception& e) {
        logError(ErrorType::InvalidPath, "1C path validation failed: " + std::string(e.what()));
        return false;
    }
}

bool ErrorHandler::validateFontPath(const std::string& path) {
    if (!validatePath(path)) {
        return false;
    }
    
    try {
        std::filesystem::path filepath(path);
        std::string extension = filepath.extension().string();
        return extension == ".ttf" || extension == ".otf" || extension == ".TTF" || extension == ".OTF";
    } catch (const std::exception& e) {
        logError(ErrorType::FontLoadFailed, "Font path validation failed: " + std::string(e.what()));
        return false;
    }
}

std::string ErrorHandler::getErrorMessage(ErrorType type) {
    switch (type) {
        case ErrorType::FileNotFound:
            return "File not found";
        case ErrorType::InvalidPath:
            return "Invalid path specified";
        case ErrorType::LaunchFailed:
            return "Failed to launch application";
        case ErrorType::FontLoadFailed:
            return "Failed to load font";
        case ErrorType::ConfigurationError:
            return "Configuration error";
        case ErrorType::EnvironmentVariableError:
            return "Environment variable error";
        case ErrorType::ProcessCreationFailed:
            return "Process creation failed";
        default:
            return "Unknown error";
    }
}

std::string ErrorHandler::getLastErrorString() {
    DWORD errorCode = GetLastError();
    if (errorCode == 0) {
        return "No error";
    }
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        nullptr
    );
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    
    return "Error " + std::to_string(errorCode) + ": " + message;
}

void ErrorHandler::setErrorCallback(ErrorCallback callback) {
    errorCallback = callback;
}

void ErrorHandler::clearErrorCallback() {
    errorCallback = nullptr;
}

void ErrorHandler::logError(ErrorType type, const std::string& details) {
    std::string message = formatErrorMessage(type, details);
    std::cerr << "[ERROR] " << message << std::endl;
    
    // Write to log file
    std::ofstream logFile("run1c_error.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[ERROR] " << message << std::endl;
        logFile.close();
    }
}

void ErrorHandler::logWarning(const std::string& message) {
    std::cerr << "[WARNING] " << message << std::endl;
    
    std::ofstream logFile("run1c_error.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[WARNING] " << message << std::endl;
        logFile.close();
    }
}

void ErrorHandler::logInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
    
    std::ofstream logFile("run1c_error.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[INFO] " << message << std::endl;
        logFile.close();
    }
}

std::string ErrorHandler::formatErrorMessage(ErrorType type, const std::string& details) {
    std::string baseMessage = getErrorMessage(type);
    if (!details.empty()) {
        return baseMessage + ": " + details;
    }
    return baseMessage;
}