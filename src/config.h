#pragma once

#include <string>
#include <optional>

class Config {
public:
    // Font configuration
    static std::string getDefaultFontPath();
    static std::string getFontPath();
    static void setCustomFontPath(const std::string& path);
    
    // 1C Enterprise configuration
    static std::string getDefault1CStarterPath();
    static std::string get1CStarterPath();
    static void setCustom1CStarterPath(const std::string& path);
    
    // Application settings
    static int getBaseFontSize();
    static void setBaseFontSize(int size);
    
    static std::string getStorageFilePath();
    static void setStorageFilePath(const std::string& path);
    
    // Validation
    static bool isValidPath(const std::string& path);
    static bool is1CStarterValid(const std::string& path);

private:
    static std::optional<std::string> customFontPath;
    static std::optional<std::string> custom1CStarterPath;
    static std::optional<std::string> customStoragePath;
    static int baseFontSize;
};