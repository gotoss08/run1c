#include "utils.h"
#include <filesystem>
#include <iostream>
#include <regex>
#include <fstream>

std::string getEnvironmentVariable(const std::string& varName) {
    // Determine the size of the buffer required
    DWORD bufferSize = GetEnvironmentVariableA(varName.c_str(), nullptr, 0);
    if (bufferSize == 0) {
        throw std::runtime_error("Failed to retrieve environment variable size. Error: " + std::to_string(GetLastError()));
    }

    // Create a buffer to hold the environment variable value
    std::vector<char> buffer(bufferSize);

    // Retrieve the environment variable value
    DWORD resultSize = GetEnvironmentVariableA(varName.c_str(), buffer.data(), bufferSize);
    if (resultSize == 0 || resultSize >= bufferSize) {
        throw std::runtime_error("Failed to retrieve environment variable. Error: " + std::to_string(GetLastError()));
    }

    // Convert the buffer to a std::string and return it
    return std::string(buffer.data());
}

void replaceWithRegex(std::string& str, const std::string& from, const std::string& to) {
    std::regex regex(from);
    str = std::regex_replace(str, regex, to);
}

void replaceSubstring(std::string& str, const std::string& from, const std::string& to) {
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length(); // Advance startPos to avoid replacing the newly inserted substring
    }
}

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.resize(size_needed - 1); // Remove the extra null terminator
    return wstr;
}

void launchProcess(std::string program, const std::vector<std::string>& args) {
    // Validate program path
    if (program.empty()) {
        throw std::runtime_error("Program path cannot be empty");
    }

    // Check if program exists
    if (!std::filesystem::exists(program)) {
        throw std::runtime_error("Program not found: " + program);
    }

    std::wstring appName = stringToWString(program);
    std::wstring commandLine = appName; // Start with the program name

    // Build command line with proper escaping
    for (const auto& arg : args) {
        commandLine += L" ";
        std::wstring warg = stringToWString(arg);
        
        // Escape arguments that contain spaces
        if (warg.find(L' ') != std::wstring::npos && 
            warg.front() != L'"' && warg.back() != L'"') {
            commandLine += L"\"" + warg + L"\"";
        } else {
            commandLine += warg;
        }
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Set working directory to program's directory
    std::filesystem::path programPath(program);
    std::wstring workingDir = stringToWString(programPath.parent_path().string());

    BOOL result = CreateProcessW(
        appName.c_str(),           // Application name
        &commandLine[0],           // Command line
        NULL,                      // Process security attributes
        NULL,                      // Thread security attributes
        FALSE,                     // Inherit handles
        0,                         // Creation flags
        NULL,                      // Environment
        workingDir.empty() ? NULL : workingDir.c_str(), // Current directory
        &si,                       // Startup info
        &pi                        // Process information
    );

    if (!result) {
        DWORD errorCode = GetLastError();
        std::string errorMsg = "CreateProcess failed with error " + std::to_string(errorCode);
        
        // Get detailed error message
        LPSTR messageBuffer = nullptr;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer,
            0,
            nullptr
        );
        
        if (messageBuffer) {
            errorMsg += ": " + std::string(messageBuffer);
            LocalFree(messageBuffer);
        }
        
        throw std::runtime_error(errorMsg);
    }

    // Wait for process to complete with timeout (30 seconds)
    DWORD waitResult = WaitForSingleObject(pi.hProcess, 30000);
    
    if (waitResult == WAIT_TIMEOUT) {
        std::cerr << "Warning: Process is taking longer than expected to start" << std::endl;
    } else if (waitResult == WAIT_FAILED) {
        std::cerr << "Warning: Failed to wait for process: " << GetLastError() << std::endl;
    }

    // Get exit code
    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        if (exitCode != 0 && exitCode != STILL_ACTIVE) {
            std::cerr << "Warning: Process exited with code: " << exitCode << std::endl;
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void createFileIfNotExists(const std::string& path) {
    std::ifstream infile(path);
    if (!infile.good()) {
        std::ofstream outfile(path);
    }
}