// -------------------------------------------------
// Sweeper.cpp
// Implementation of the Sweeper controller class.
// Recursively traverses a directory tree, gathers
// FileID records, identifies duplicate files by
// iNode and by content comparison, and reports
// all duplicates found.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "Sweeper.hpp"
#include "tools.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <climits>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

// -------------------------------------------------
// Constructor: store a reference to the Params
// object so its settings are available later.
// Parameters:
//   par - reference to the Params object.
// -------------------------------------------------
Sweeper::Sweeper(Params& par)
    : params(par) {}

// -------------------------------------------------
// joinPath(): Combine a base directory path and
// a file or directory name into a single path.
// Avoids doubling the '/' separator.
// Parameters:
//   base - absolute path of the directory.
//   name - name of the entry to append.
// Returns: the combined pathname string.
// -------------------------------------------------
string Sweeper::joinPath(
    const string& base,
    const string& name) {
    if (base.empty()) return name;
    if (base.back() == '/')
        return base + name;
    return base + "/" + name;
}

// -------------------------------------------------
// filesMatch(): Open two files and compare them
// byte by byte.  Returns true if the contents
// are identical, false on the first mismatch
// or on any read error.
// Parameters:
//   pathA - absolute path to the first file.
//   pathB - absolute path to the second file.
// Returns: true if contents are identical.
// -------------------------------------------------
bool Sweeper::filesMatch(
    const string& pathA,
    const string& pathB) {

    ifstream streamA(pathA, ios::binary);
    ifstream streamB(pathB, ios::binary);

    // If either file cannot be opened,
    // treat them as non-matching.
    if (!streamA.is_open()
        || !streamB.is_open()) {
        return false;
    }

    // Read and compare one character at a time.
    // Stop on the first mismatch.
    char charA, charB;
    while (streamA.get(charA)) {
        if (!streamB.get(charB))
            return false;  // B ended early.
        if (charA != charB)
            return false;  // Mismatch found.
    }

    // If A is exhausted, B should be too.
    // If B still has data, files differ.
    if (streamB.get(charB))
        return false;

    return true;
}

// -------------------------------------------------
// travel(): Recursively traverse a directory
// tree.  Opens the directory by absolute path,
// reads each entry, and processes regular files
// and subdirectories.
// Parameters:
//   absPath - absolute pathname of the
//             directory to process.
// -------------------------------------------------
void Sweeper::travel(const string& absPath) {
    ostream& out = params.getOut();

    // Open the directory using absolute path.
    DIR* dirPtr = opendir(absPath.c_str());
    if (dirPtr == nullptr) {
        cerr << "Error: cannot open "
             << "directory: " << absPath
             << " (" << strerror(errno)
             << ")" << endl;
        return;
    }

    out << "\n[DIR] " << absPath << endl;

    struct dirent* rawEntry;
    while ((rawEntry = readdir(dirPtr))
           != nullptr) {
        Direntry* entry =
            reinterpret_cast<Direntry*>(
                rawEntry);
        string name = entry->getName();

        // Skip current and parent directory
        // entries to avoid infinite recursion.
        if (name == "." || name == "..")
            continue;

        // Build the full path for lstat.
        string fullPath =
            joinPath(absPath, name);

        // Use lstat on the absolute path so
        // we can classify files and links.
        Stats statBuf;
        if (lstat(fullPath.c_str(),
                  &statBuf) != 0) {
            cerr << "Warning: lstat failed "
                 << "for " << fullPath
                 << " (" << strerror(errno)
                 << ")" << endl;
            continue;
        }

        bool isDir =
            S_ISDIR(statBuf.st_mode);
        bool isReg =
            S_ISREG(statBuf.st_mode);
        bool isLink =
            S_ISLNK(statBuf.st_mode);

        // Verbose ON: print entry info.
        // Verbose OFF: skip links entirely.
        if (params.isVerbose()) {
            entry->print(out);
        } else {
            if (isLink) continue;
        }

        // If entry is a directory, recurse.
        if (isDir) {
            travel(fullPath);
            continue;
        }

        // Process regular files only.
        if (!isReg) continue;

        // Apply the minimum file size filter.
        if (statBuf.st_size
            < params.getMinFileSize())
            continue;

        // Store FileID for this regular file.
        fileList.emplace_back(
            name, absPath,
            entry->getInode(),
            statBuf.st_size);
    }

    closedir(dirPtr);
}

// -------------------------------------------------
// sortByLength(): Sort the FileID vector by
// file length using the global compareByLength
// comparator function.
// -------------------------------------------------
void Sweeper::sortByLength() {
    sort(fileList.begin(),
         fileList.end(),
         compareByLength);
}

// -------------------------------------------------
// printFileList(): Print the full list of FileID
// records in formatted columns.
// Parameters:
//   out - the output stream to write to.
// -------------------------------------------------
void Sweeper::printFileList(
    ostream& out) const {
    out << "\n--- File List ("
        << fileList.size()
        << " files) ---\n";
    out << setw(10) << "Length"
        << "\t" << setw(10) << "iNode"
        << "\tPathname\n";
    out << setw(10) << "------"
        << "\t" << setw(10) << "-----"
        << "\t--------\n";

    for (const FileID& file : fileList) {
        out << setw(10) << file.getLength()
            << "\t" << setw(10)
            << file.getInode()
            << "\t" << file.getPath()
            << "\n";
    }
    out << "--- End of File List ---\n";
}

// -------------------------------------------------
// findDups(): Identify duplicate files using
// two passes with control-break logic.
//
// Pass 1: Sort by iNode number (using the
//   member operator<).  Consecutive entries
//   with the same iNode are hard links to the
//   same underlying file.
//
// Pass 2: Sort by file length (using the
//   global compareByLength function).  For
//   groups of same-length files, compare
//   contents byte by byte.  Files with
//   identical contents are true duplicates.
//
// Parameter:
//   fileVec - vector of FileIDs to scan.
// -------------------------------------------------
void Sweeper::findDups(
    vector<FileID>& fileVec) {

    ostream& out = params.getOut();

    // =========================================
    // Pass 1: Find hard-link duplicates
    //         (same iNode number).
    // =========================================
    out << "\n==============================="
        << "======\n";
    out << "Pass 1: Checking for shared "
        << "iNodes (hard links)\n";
    out << "==============================="
        << "======\n";

    // Sort using member operator< which
    // compares by iNode number.
    sort(fileVec.begin(), fileVec.end());

    bool foundInodeDups = false;

    // Control-break logic: walk through the
    // sorted vector and detect consecutive
    // entries that share the same iNode.
    size_t idx = 0;
    while (idx < fileVec.size()) {
        // Start of a new iNode group.
        ino_t currentInode =
            fileVec[idx].getInode();
        size_t groupStart = idx;

        // Advance past all entries with
        // the same iNode number.
        while (idx < fileVec.size()
               && fileVec[idx].getInode()
                  == currentInode) {
            idx++;
        }

        // If the group has more than one
        // entry, they are hard links to the
        // same file.
        size_t groupSize = idx - groupStart;
        if (groupSize > 1) {
            foundInodeDups = true;
            out << "\n  iNode "
                << currentInode
                << " is shared by "
                << groupSize
                << " files:\n";
            for (size_t pos = groupStart;
                 pos < idx; pos++) {
                out << "    ";
                fileVec[pos].print(out);
            }
        }
    }

    if (!foundInodeDups) {
        out << "\n  No shared iNodes "
            << "found.\n";
    }

    // =========================================
    // Pass 2: Find content-identical files
    //         (same length, same contents).
    // =========================================
    out << "\n==============================="
        << "======\n";
    out << "Pass 2: Checking for identical "
        << "contents (by length)\n";
    out << "==============================="
        << "======\n";

    // Sort by file length using the global
    // comparator function.
    sort(fileVec.begin(), fileVec.end(),
         compareByLength);

    bool foundContentDups = false;

    // Control-break logic: walk through the
    // length-sorted vector and detect groups
    // of files with the same length.
    idx = 0;
    while (idx < fileVec.size()) {
        // Start of a new length group.
        off_t currentLen =
            fileVec[idx].getLength();
        size_t groupStart = idx;

        // Advance past all entries with
        // the same file length.
        while (idx < fileVec.size()
               && fileVec[idx].getLength()
                  == currentLen) {
            idx++;
        }

        size_t groupSize = idx - groupStart;

        // Only compare if two or more files
        // share the same length.
        if (groupSize < 2) continue;

        // Compare every pair within this
        // same-length group byte by byte.
        // Report pairs that match.
        for (size_t outer = groupStart;
             outer < idx - 1; outer++) {
            for (size_t inner = outer + 1;
                 inner < idx; inner++) {

                // Skip pairs that share an
                // iNode: they were already
                // reported in Pass 1.
                if (fileVec[outer].getInode()
                    == fileVec[inner]
                       .getInode()) {
                    continue;
                }

                // Compare file contents.
                if (filesMatch(
                    fileVec[outer].getPath(),
                    fileVec[inner]
                        .getPath())) {

                    foundContentDups = true;
                    out << "\n  Content "
                        << "match (length "
                        << currentLen
                        << "):\n";
                    out << "    ";
                    fileVec[outer]
                        .print(out);
                    out << "    ";
                    fileVec[inner]
                        .print(out);
                }
            }
        }
    }

    if (!foundContentDups) {
        out << "\n  No content-identical "
            << "duplicates found.\n";
    }

    out << "\n==============================="
        << "======\n";
    out << "End of duplicate report.\n";
    out << "==============================="
        << "======\n";
}
