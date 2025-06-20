#include "merge.hpp"
#include "commit.hpp"
#include "fileio.hpp"

#include <set>
#include <queue>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

std::string findLCA(const std::string& ourHash, const std::string& theirHash) {
    std::set<std::string> visited;
    std::queue<std::string> q;
    q.push(ourHash);

    while (!q.empty()) {
        std::string current = q.front(); q.pop();
        if (visited.count(current)) continue;
        visited.insert(current);

        Commit c = readCommit(current);
        if (!c.parent1.empty()) q.push(c.parent1);
        if (!c.parent2.empty()) q.push(c.parent2);
    }

    q.push(theirHash);
    while (!q.empty()) {
        std::string current = q.front(); q.pop();
        if (visited.count(current)) return current;

        Commit c = readCommit(current);
        if (!c.parent1.empty()) q.push(c.parent1);
        if (!c.parent2.empty()) q.push(c.parent2);
    }

    return "";
}

std::vector<std::string> mergeFiles(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs,
    bool& conflict
) {
    std::vector<std::string> merged;
    conflict = false;
    size_t n = std::max({ base.size(), ours.size(), theirs.size() });

    for (size_t i = 0; i < n; ++i) {
        std::string b = i < base.size() ? base[i] : "";
        std::string o = i < ours.size() ? ours[i] : "";
        std::string t = i < theirs.size() ? theirs[i] : "";

        if (o == t) merged.push_back(o);
        else if (b == o) merged.push_back(t);
        else if (b == t) merged.push_back(o);
        else {
            merged.push_back("<<<<<<< OUR VERSION");
            merged.push_back(o);
            merged.push_back("=======");
            merged.push_back(t);
            merged.push_back(">>>>>>> THEIR VERSION");
            conflict = true;
        }
    }

    return merged;
}

void mergeBranches(const std::string& ourHash, const std::string& theirHash) {
    std::string baseHash = findLCA(ourHash, theirHash);
    if (baseHash.empty()) {
        std::cout << "No common ancestor found.\n";
        return;
    }

    Commit base = readCommit(baseHash);
    Commit ours = readCommit(ourHash);
    Commit theirs = readCommit(theirHash);

    std::set<std::string> allFiles;

    // Collect all unique file names from base, ours, and theirs
    for (auto it = base.fileToBlob.begin(); it != base.fileToBlob.end(); ++it)
        allFiles.insert(it->first);
    for (auto it = ours.fileToBlob.begin(); it != ours.fileToBlob.end(); ++it)
        allFiles.insert(it->first);
    for (auto it = theirs.fileToBlob.begin(); it != theirs.fileToBlob.end(); ++it)
        allFiles.insert(it->first);

    // Merge files
    for (auto it = allFiles.begin(); it != allFiles.end(); ++it) {
        std::string file = *it;

        std::vector<std::string> baseLines, ourLines, theirLines;
        if (base.fileToBlob.count(file)) baseLines = readFileLines(base.fileToBlob[file]);
        if (ours.fileToBlob.count(file)) ourLines = readFileLines(ours.fileToBlob[file]);
        if (theirs.fileToBlob.count(file)) theirLines = readFileLines(theirs.fileToBlob[file]);

        bool conflict = false;
        std::vector<std::string> merged = mergeFiles(baseLines, ourLines, theirLines, conflict);
        writeFileLines(file, merged);

        if (conflict) {
            std::cout << "CONFLICT: both modified " << file << "\n";
        }
    }
}
