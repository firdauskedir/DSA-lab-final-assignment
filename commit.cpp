#include "commit.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Commit readCommit(const std::string& commitHash) {
    Commit commit;
    commit.hash = commitHash;
    std::ifstream fin(".minigit/commits/" + commitHash + ".txt");


    if (!fin) {
        std::cerr << "Could not read commit " << commitHash << "\n";
        return commit;
    }

    std::string line;
    while (getline(fin, line)) {
        if (line.find("parent:") == 0) {
            std::string parentLine = line.substr(7);
            while (!parentLine.empty() && parentLine[0] == ' ') parentLine.erase(0, 1);  

            size_t comma = parentLine.find(',');
            if (comma != std::string::npos) {
                commit.parent1 = parentLine.substr(0, comma);
                commit.parent2 = parentLine.substr(comma + 1);
            }
            else {
                commit.parent1 = parentLine;
            }
        }
        else if (line == "files:") {
            while (getline(fin, line)) {
                size_t sep = line.find(':');
                if (sep != std::string::npos) {
                    std::string file = line.substr(0, sep);
                    std::string blob = line.substr(sep + 1);
                    commit.fileToBlob[file] = blob;
                }
            }
        }
    }

    return commit;
}