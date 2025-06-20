#include "fileio.hpp"
#include <fstream>

std::vector<std::string> readFileLines(const std::string& blobHash) {
    std::ifstream fin(".minigit/objects/" + blobHash);
    std::vector<std::string> lines;
    std::string line;
    while (getline(fin, line)) {
        lines.push_back(line);
    }
    return lines;
}
void writeFileLines(const std::string& filepath, const std::vector<std::string>& lines) {
    std::ofstream fout(filepath);
    for (const std::string& line : lines) {
        fout << line << "\n";
    }
}