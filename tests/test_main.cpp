#include <gtest/gtest.h>
#include <Windows.h>

int main(int argc, char **argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests and return the result
    return RUN_ALL_TESTS();
}