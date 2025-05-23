#include <gtest/gtest.h>
#include "error_handler.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>

class ErrorHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Backup original error callback
        originalCallback = nullptr;
        
        // Create test files
        testFontPath = "test_font.ttf";
        std::ofstream fontFile(testFontPath);
        fontFile.close();
        
        test1CStarterPath = "1cestart.exe";
        std::ofstream starterFile(test1CStarterPath);
        starterFile.close();
        
        // Remove test log file if it exists
        if (std::filesystem::exists("run1c_error.log")) {
            std::filesystem::remove("run1c_error.log");
        }
    }

    void TearDown() override {
        // Reset error callback
        ErrorHandler::clearErrorCallback();
        
        // Remove test files
        if (std::filesystem::exists(testFontPath)) {
            std::filesystem::remove(testFontPath);
        }
        if (std::filesystem::exists(test1CStarterPath)) {
            std::filesystem::remove(test1CStarterPath);
        }
        if (std::filesystem::exists("run1c_error.log")) {
            std::filesystem::remove("run1c_error.log");
        }
    }

    ErrorHandler::ErrorCallback originalCallback;
    std::string testFontPath;
    std::string test1CStarterPath;
    
    // Helper to check if log file contains specific text
    bool logFileContains(const std::string& text) {
        std::ifstream log("run1c_error.log");
        if (!log.is_open()) return false;
        
        std::string line;
        while (std::getline(log, line)) {
            if (line.find(text) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

TEST_F(ErrorHandlerTest, ValidatePathTest) {
    EXPECT_TRUE(ErrorHandler::validatePath(testFontPath));
    EXPECT_FALSE(ErrorHandler::validatePath("non_existent_file.txt"));
    EXPECT_FALSE(ErrorHandler::validatePath(""));
}

TEST_F(ErrorHandlerTest, Validate1CPathTest) {
    EXPECT_TRUE(ErrorHandler::validate1CPath(test1CStarterPath));
    
    // Test with invalid file name
    std::string invalidPath = "invalid_file.exe";
    std::ofstream invalidFile(invalidPath);
    invalidFile.close();
    EXPECT_FALSE(ErrorHandler::validate1CPath(invalidPath));
    std::filesystem::remove(invalidPath);
    
    // Test with non-existent file
    EXPECT_FALSE(ErrorHandler::validate1CPath("non_existent_file.exe"));
}

TEST_F(ErrorHandlerTest, ValidateFontPathTest) {
    EXPECT_TRUE(ErrorHandler::validateFontPath(testFontPath));
    
    // Test with invalid extension
    std::string invalidPath = "invalid_font.txt";
    std::ofstream invalidFile(invalidPath);
    invalidFile.close();
    EXPECT_FALSE(ErrorHandler::validateFontPath(invalidPath));
    std::filesystem::remove(invalidPath);
    
    // Test with non-existent file
    EXPECT_FALSE(ErrorHandler::validateFontPath("non_existent_font.ttf"));
}

TEST_F(ErrorHandlerTest, GetErrorMessageTest) {
    EXPECT_EQ(ErrorHandler::getErrorMessage(ErrorType::FileNotFound), "File not found");
    EXPECT_EQ(ErrorHandler::getErrorMessage(ErrorType::InvalidPath), "Invalid path specified");
    EXPECT_EQ(ErrorHandler::getErrorMessage(ErrorType::LaunchFailed), "Failed to launch application");
    EXPECT_EQ(ErrorHandler::getErrorMessage(ErrorType::FontLoadFailed), "Failed to load font");
}

TEST_F(ErrorHandlerTest, ErrorCallbackTest) {
    bool callbackCalled = false;
    ErrorType capturedType = ErrorType::FileNotFound;
    std::string capturedDetails;
    
    ErrorHandler::setErrorCallback([&](ErrorType type, const std::string& details) {
        callbackCalled = true;
        capturedType = type;
        capturedDetails = details;
    });
    
    ErrorHandler::showError(ErrorType::InvalidPath, "Test details");
    
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(capturedType, ErrorType::InvalidPath);
    EXPECT_EQ(capturedDetails, "Test details");
    
    // Clear callback and test again
    ErrorHandler::clearErrorCallback();
    callbackCalled = false;
    
    ErrorHandler::showError(ErrorType::FileNotFound, "Another test");
    EXPECT_FALSE(callbackCalled); // Should not be called after clearing
}

TEST_F(ErrorHandlerTest, LogErrorTest) {
    ErrorHandler::logError(ErrorType::LaunchFailed, "Test error message");
    EXPECT_TRUE(logFileContains("[ERROR]"));
    EXPECT_TRUE(logFileContains("Failed to launch application"));
    EXPECT_TRUE(logFileContains("Test error message"));
}

TEST_F(ErrorHandlerTest, LogWarningTest) {
    ErrorHandler::logWarning("Test warning message");
    EXPECT_TRUE(logFileContains("[WARNING]"));
    EXPECT_TRUE(logFileContains("Test warning message"));
}

TEST_F(ErrorHandlerTest, LogInfoTest) {
    ErrorHandler::logInfo("Test info message");
    EXPECT_TRUE(logFileContains("[INFO]"));
    EXPECT_TRUE(logFileContains("Test info message"));
}

TEST_F(ErrorHandlerTest, GetLastErrorStringTest) {
    // Set last error
    SetLastError(ERROR_FILE_NOT_FOUND);
    std::string errorMsg = ErrorHandler::getLastErrorString();
    EXPECT_TRUE(errorMsg.find("Error 2") != std::string::npos);
}