#include <gtest/gtest.h>
#include "utils.h"
#include <filesystem>
#include <fstream>
#include <string>

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test file
        testFilePath = "test_file.txt";
        std::ofstream outfile(testFilePath);
        outfile << "Test content" << std::endl;
        outfile.close();
    }

    void TearDown() override {
        // Remove temporary test file
        if (std::filesystem::exists(testFilePath)) {
            std::filesystem::remove(testFilePath);
        }
    }

    std::string testFilePath;
};

TEST_F(UtilsTest, ReplaceWithRegexTest) {
    std::string testStr = "Hello world! This is a test string.";
    replaceWithRegex(testStr, "\\btest\\b", "sample");
    EXPECT_EQ(testStr, "Hello world! This is a sample string.");
}

TEST_F(UtilsTest, ReplaceSubstringTest) {
    std::string testStr = "Hello world! This is a test string with test word.";
    replaceSubstring(testStr, "test", "sample");
    EXPECT_EQ(testStr, "Hello world! This is a sample string with sample word.");
}

TEST_F(UtilsTest, StringToWStringTest) {
    std::string testStr = "Hello world!";
    std::wstring wstr = stringToWString(testStr);
    
    // Convert back to string for comparison
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, NULL, NULL);
    result.resize(size_needed - 1); // Remove null terminator
    
    EXPECT_EQ(result, testStr);
}

TEST_F(UtilsTest, CreateFileIfNotExistsTest) {
    std::string newFilePath = "new_test_file.txt";
    
    // Make sure the file doesn't exist
    if (std::filesystem::exists(newFilePath)) {
        std::filesystem::remove(newFilePath);
    }
    
    // Test creating a new file
    createFileIfNotExists(newFilePath);
    EXPECT_TRUE(std::filesystem::exists(newFilePath));
    
    // Cleanup
    std::filesystem::remove(newFilePath);
}

TEST_F(UtilsTest, CreateFileIfNotExistsExistingFileTest) {
    // The file already exists from SetUp
    long long originalSize = std::filesystem::file_size(testFilePath);
    
    // Call function on existing file
    createFileIfNotExists(testFilePath);
    
    // File should still exist with the same size
    EXPECT_TRUE(std::filesystem::exists(testFilePath));
    EXPECT_EQ(std::filesystem::file_size(testFilePath), originalSize);
}

// Skip testing launchProcess as it's more complex and requires mocks for process creation
// Skip testing getEnvironmentVariable as it depends on the system environment