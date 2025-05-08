#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>

#include <Windows.h>

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

    std::wstring appName = stringToWString(program);
    std::wstring commandLine;

    for (const auto& arg : args) {
        commandLine += L" ";
        commandLine += stringToWString(arg);
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(&appName[0], &commandLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        exit(0);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

}

void createFileIfNotExists(const std::string& path) {
    std::ifstream infile(path);
    if (!infile.good()) {
        std::ofstream outfile(path);
    }
}
