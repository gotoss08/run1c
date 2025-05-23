#pragma once

#include <string>
#include <functional>

enum class ErrorType {
    FileNotFound,
    InvalidPath,
    LaunchFailed,
    FontLoadFailed,
    ConfigurationError,
    EnvironmentVariableError,
    ProcessCreationFailed
};

class ErrorHandler {
public:
    using ErrorCallback = std::function<void(ErrorType, const std::string&)>;
    
    // Static methods for immediate error handling
    static void showError(ErrorType type, const std::string& details);
    static void showErrorWithDialog(ErrorType type, const std::string& details);
    
    // Validation methods
    static bool validatePath(const std::string& path);
    static bool validate1CPath(const std::string& path);
    static bool validateFontPath(const std::string& path);
    
    // Error reporting
    static std::string getErrorMessage(ErrorType type);
    static std::string getLastErrorString();
    
    // Error callback management
    static void setErrorCallback(ErrorCallback callback);
    static void clearErrorCallback();
    
    // Logging
    static void logError(ErrorType type, const std::string& details);
    static void logWarning(const std::string& message);
    static void logInfo(const std::string& message);

private:
    static ErrorCallback errorCallback;
    static std::string formatErrorMessage(ErrorType type, const std::string& details);
};