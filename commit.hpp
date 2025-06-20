
#pragma once
#include <unordered_map>
#include <string>

struct Commit {
    std::string hash;
    std::string parent1;
    std::string parent2; // optional
    std::unordered_map<std::string, std::string> fileToBlob;
};

Commit readCommit(const std::string& commitHash);