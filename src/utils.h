#pragma once

#include <string>
#include <vector>

#include <Windows.h>

std::string getEnvironmentVariable(const std::string& varName);
void replaceWithRegex(std::string& str, const std::string& from, const std::string& to);
void replaceSubstring(std::string& str, const std::string& from, const std::string& to);
std::wstring stringToWString(const std::string& str);
void launchProcess(std::string program, const std::vector<std::string>& args);
void createFileIfNotExists(const std::string& path);