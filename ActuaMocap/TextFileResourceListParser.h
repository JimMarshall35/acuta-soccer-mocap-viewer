#pragma once
#include <string>
#include <functional>

using ParseResourcesTextFileCallback = std::function<void(std::string, std::string)>; // params are: file path, identifier
void ParseResourcesTextFile(std::string filePath, ParseResourcesTextFileCallback callback);