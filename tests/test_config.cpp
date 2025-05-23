#include <gtest/gtest.h>
#include "config.h"
#include <filesystem>
#include <fstream>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Save original values
        originalFontPath = Config::getFontPath();
        original1CStarterPath = Config::get1CStarterPath();
        originalBaseFontSize = Config::getBaseFontSize();
        originalStoragePath = Config::getStorageFilePath();
        
        // Create test files
        testFontPath = "test_font.ttf";
        std::ofstream fontFile(testFontPath);
        fontFile.close();
        
        test1CStarterPath = "1cestart.exe";
        std::ofstream starterFile(test1CStarterPath);
        starterFile.close();
    }

    void TearDown() override {
        // Reset to original values
        if (!originalFontPath.empty()) Config::setCustomFontPath(originalFontPath);
        if (!original1CStarterPath.empty()) Config::setCustom1CStarterPath(original1CStarterPath);
        Config::setBaseFontSize(originalBaseFontSize);
        if (!originalStoragePath.empty()) Config::setStorageFilePath(originalStoragePath);
        
        // Remove test files
        if (std::filesystem::exists(testFontPath)) {
            std::filesystem::remove(testFontPath);
        }
        if (std::filesystem::exists(test1CStarterPath)) {
            std::filesystem::remove(test1CStarterPath);
        }
    }

    std::string originalFontPath;
    std::string original1CStarterPath;
    int originalBaseFontSize;
    std::string originalStoragePath;
    std::string testFontPath;
    std::string test1CStarterPath;
};

TEST_F(ConfigTest, DefaultFontPathTest) {
    std::string defaultPath = Config::getDefaultFontPath();
    EXPECT_FALSE(defaultPath.empty());
    // Default path is usually C:\Windows\Fonts\segoeui.ttf
    EXPECT_NE(defaultPath.find("Fonts"), std::string::npos);
}

TEST_F(ConfigTest, CustomFontPathTest) {
    Config::setCustomFontPath(testFontPath);
    EXPECT_EQ(Config::getFontPath(), testFontPath);
    
    // Test with invalid path
    Config::setCustomFontPath("non_existent_font.ttf");
    // Should fall back to default
    EXPECT_NE(Config::getFontPath(), "non_existent_font.ttf");
}

TEST_F(ConfigTest, Default1CStarterPathTest) {
    std::string defaultPath = Config::getDefault1CStarterPath();
    EXPECT_FALSE(defaultPath.empty());
    // Default path should contain 1cv8 and 1cestart.exe
    EXPECT_NE(defaultPath.find("1cv8"), std::string::npos);
    EXPECT_NE(defaultPath.find("1cestart.exe"), std::string::npos);
}

TEST_F(ConfigTest, Custom1CStarterPathTest) {
    Config::setCustom1CStarterPath(test1CStarterPath);
    EXPECT_EQ(Config::get1CStarterPath(), test1CStarterPath);
    
    // Test with invalid path
    Config::setCustom1CStarterPath("non_existent_1cestart.exe");
    // Should fall back to default
    EXPECT_NE(Config::get1CStarterPath(), "non_existent_1cestart.exe");
}

TEST_F(ConfigTest, BaseFontSizeTest) {
    int newSize = 24;
    Config::setBaseFontSize(newSize);
    EXPECT_EQ(Config::getBaseFontSize(), newSize);
    
    // Test with invalid size (too small)
    Config::setBaseFontSize(4);
    // Should remain unchanged
    EXPECT_EQ(Config::getBaseFontSize(), newSize);
    
    // Test with invalid size (too large)
    Config::setBaseFontSize(80);
    // Should remain unchanged
    EXPECT_EQ(Config::getBaseFontSize(), newSize);
}

TEST_F(ConfigTest, StorageFilePathTest) {
    std::string newPath = "custom_storage.ini";
    Config::setStorageFilePath(newPath);
    EXPECT_EQ(Config::getStorageFilePath(), newPath);
    
    // Default path should contain user profile or local app data
    std::string defaultPath = Config::getStorageFilePath();
    EXPECT_FALSE(defaultPath.empty());
}

TEST_F(ConfigTest, ValidPathTest) {
    // Test with existing file
    EXPECT_TRUE(Config::isValidPath(testFontPath));
    
    // Test with non-existent file
    EXPECT_FALSE(Config::isValidPath("non_existent_file.txt"));
    
    // Test with empty path
    EXPECT_FALSE(Config::isValidPath(""));
}

TEST_F(ConfigTest, Is1CStarterValidTest) {
    // Test with valid file name
    EXPECT_TRUE(Config::is1CStarterValid(test1CStarterPath));
    
    // Test with invalid file name
    std::string invalidPath = "invalid_file.exe";
    std::ofstream invalidFile(invalidPath);
    invalidFile.close();
    EXPECT_FALSE(Config::is1CStarterValid(invalidPath));
    std::filesystem::remove(invalidPath);
    
    // Test with non-existent file
    EXPECT_FALSE(Config::is1CStarterValid("non_existent_file.exe"));
}