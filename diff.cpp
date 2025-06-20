#include "diff.hpp"
#include "commit.hpp"
#include "fileio.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>

void diffLines(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j]) {
            ++i; ++j;
        }
        else {
            std::cout << "- " << a[i++] << "\n";
            std::cout << "+ " << b[j++] << "\n";
        }
    }
    while (i < a.size()) std::cout << "- " << a[i++] << "\n";
    while (j < b.size()) std::cout << "+ " << b[j++] << "\n";
}

void diffCommits(const std::string& hash1, const std::string& hash2) {
    Commit c1 = readCommit(hash1);
    Commit c2 = readCommit(hash2);

    for (auto it = c1.fileToBlob.begin(); it != c1.fileToBlob.end(); ++it) {
        std::string file = it->first;
        std::string blob1 = it->second;

        if (c2.fileToBlob.count(file)) {
            std::string blob2 = c2.fileToBlob[file];
            if (blob1 == blob2) continue;

            std::vector<std::string> a = readFileLines(blob1);
            std::vector<std::string> b = readFileLines(blob2);

            std::cout << "Diff for " << file << ":\n";
            std::cout << "[DEBUG] Blob1 lines:\n";
            for (const std::string& line : a) std::cout << line << "\n";

            std::cout << "[DEBUG] Blob2 lines:\n";
            for (const std::string& line : b) std::cout << line << "\n";
            diffLines(a, b);
            std::cout << "--------------------\n";
        }
    }
}