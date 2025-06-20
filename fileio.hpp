
#pragma once
#include <string>
#include <vector>

std::vector<std::string> readFileLines(const std::string& blobHash);
void writeFileLines(const std::string& filepath, const std::vector<std::string>& lines);