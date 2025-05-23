#include "config.h"
#include "utils.h"
#include <filesystem>

// Static member definitions
std::optional<std::string> Config::customFontPath;
std::optional<std::string> Config::custom1CStarterPath;
std::optional<std::string> Config::customStoragePath;
int Config::baseFontSize = 18;

std::string Config::getDefaultFontPath() {
    return "C:\\Windows\\Fonts\\segoeui.ttf";
}

std::string Config::getFontPath() {
    if (customFontPath.has_value() && isValidPath(customFontPath.value())) {
        return customFontPath.value();
    }
    return getDefaultFontPath();
}

void Config::setCustomFontPath(const std::string& path) {
    if (isValidPath(path)) {
        customFontPath = path;
    }
}

std::string Config::getDefault1CStarterPath() {
    try {
        return getEnvironmentVariable("PROGRAMFILES") + "\\1cv8\\common\\1cestart.exe";
    } catch (const std::exception&) {
        return "C:\\Program Files\\1cv8\\common\\1cestart.exe";
    }
}

std::string Config::get1CStarterPath() {
    if (custom1CStarterPath.has_value() && is1CStarterValid(custom1CStarterPath.value())) {
        return custom1CStarterPath.value();
    }
    return getDefault1CStarterPath();
}

void Config::setCustom1CStarterPath(const std::string& path) {
    if (is1CStarterValid(path)) {
        custom1CStarterPath = path;
    }
}

int Config::getBaseFontSize() {
    return baseFontSize;
}

void Config::setBaseFontSize(int size) {
    if (size >= 8 && size <= 72) {
        baseFontSize = size;
    }
}

std::string Config::getStorageFilePath() {
    if (customStoragePath.has_value()) {
        return customStoragePath.value();
    }
    
    #ifdef _WIN32
    // Windows: Use %LOCALAPPDATA%
    try {
        std::string appdata = getEnvironmentVariable("LOCALAPPDATA");
        return appdata + "\\RUN1C\\run1c_storage.ini";
    } catch(...) {
        // Fallback to %USERPROFILE% if LOCALAPPDATA not available
        try {
            std::string userprofile = getEnvironmentVariable("USERPROFILE");
            return userprofile + "\\AppData\\Local\\RUN1C\\run1c_storage.ini";
        } catch(...) {
            // Fallback to current directory
        }
    }
    #elif defined(__APPLE__)
    // macOS: Use ~/Library/Application Support/
    const char* home = std::getenv("HOME");
    if (home != nullptr) {
        return std::string(home) + "/Library/Application Support/RUN1C/run1c_storage.ini";
    }
    #else
    // Linux/Unix: Use ~/.config/
    const char* home = std::getenv("HOME");
    if (home != nullptr) {
        return std::string(home) + "/.config/run1c/run1c_storage.ini";
    }
    #endif
    
    // Fallback to current directory
    return "run1c_storage.ini";
}

void Config::setStorageFilePath(const std::string& path) {
    customStoragePath = path;
}


bool Config::isValidPath(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    try {
        return std::filesystem::exists(path);
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::is1CStarterValid(const std::string& path) {
    if (!isValidPath(path)) {
        return false;
    }
    
    // Check if the file ends with 1cestart.exe
    std::filesystem::path filepath(path);
    return filepath.filename() == "1cestart.exe";
}