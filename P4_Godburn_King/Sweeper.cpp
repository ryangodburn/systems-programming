// -------------------------------------------------
// Sweeper.cpp
// Implementation of the Sweeper controller class.
// Recursively traverses a directory tree, gathers
// FileID records for every regular file found,
// and can sort and print the results.
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
#include <iomanip>
#include <iostream>

using namespace std;

// -------------------------------------------------
// Constructor: store a reference to the Params
// object so its settings are available later.
// Parameters:
//   par - reference to the Params object.
// -------------------------------------------------
Sweeper::Sweeper(Params& par) : params(par) {}

// -------------------------------------------------
// joinPath(): Combine a base directory path and
// a file or directory name into a single path.
// Avoids doubling the '/' separator.
// Parameters:
//   base - absolute path of the directory.
//   name - name of the entry to append.
// Returns: the combined pathname string.
// -------------------------------------------------
string Sweeper::joinPath(const string& base, const string& name) {
    if (base.empty()) return name;
    if (base.back() == '/') return base + name;
    return base + "/" + name;
}

// -------------------------------------------------
// travel(): Recursively traverse a directory tree.
// Opens the directory by absolute path, reads
// each entry, and processes regular files and
// subdirectories.  Only chdirs into child
// directories before making recursive calls, as
// required by the assignment.
// Parameters:
//   absPath - absolute pathname of the directory
//             to process.
// -------------------------------------------------
void Sweeper::travel(const string& absPath) {
    ostream& out = params.getOut();

    // Open the directory using the absolute path.
    DIR* dirPtr = opendir(absPath.c_str());
    if (dirPtr == nullptr) {
        cerr << "Error: cannot open directory: " << absPath
             << " (" << strerror(errno) << ")" << endl;
        return;
    }

    out << "\n[DIR] " << absPath << endl;

    struct dirent* rawEntry;
    while ((rawEntry = readdir(dirPtr)) != nullptr) {
        Direntry* entry = reinterpret_cast<Direntry*>(rawEntry);
        string name = entry->getName();

        // Skip the current and parent directory
        // entries to avoid infinite recursion.
        if (name == "." || name == "..") continue;

        // Build the full absolute path for lstat.
        string fullPath = joinPath(absPath, name);

        // Use lstat on the absolute path so we
        // can reliably classify files, links,
        // and directories.
        Stats st;
        if (lstat(fullPath.c_str(), &st) != 0) {
            cerr << "Warning: lstat failed for " << fullPath
                 << " (" << strerror(errno) << ")" << endl;
            continue;
        }

        bool isDir  = S_ISDIR(st.st_mode);
        bool isReg  = S_ISREG(st.st_mode);
        bool isLink = S_ISLNK(st.st_mode);

        // Verbose ON: print name, inode, and
        // whether it is a file or a link.
        // Verbose OFF: ignore links entirely.
        if (params.isVerbose()) {
            entry->print(out);
        } else {
            if (isLink) continue;
        }

        // If the entry is a directory, cd into it
        // and make a recursive call on travel().
        if (isDir) {
            travel(fullPath);
            continue;
        }

        // Process regular files only; ignore
        // other types (devices, sockets, etc.).
        if (!isReg) continue;

        // Apply the minimum file size filter.
        if (st.st_size < params.getMinFileSize()) continue;

        // Store FileID for this regular file.
        fileList.emplace_back(name, absPath, entry->getInode(), st.st_size);
    }

    closedir(dirPtr);
}

// -------------------------------------------------
// sortByLength(): Sort the collected FileID
// vector by file length in ascending order.
// Ties are broken by pathname for stable output.
// -------------------------------------------------
void Sweeper::sortByLength() {
    sort(fileList.begin(), fileList.end(),
         [](const FileID& a, const FileID& b) {
             if (a.getLength() != b.getLength())
                 return a.getLength() < b.getLength();
             return a.getPath() < b.getPath();
         });
}

// -------------------------------------------------
// printFileList(): Print the full list of FileID
// records in formatted columns.
// Parameters:
//   out - the output stream to write to.
// -------------------------------------------------
void Sweeper::printFileList(ostream& out) const {
    out << "\n--- File List (" << fileList.size() << " files) ---\n";
    out << setw(10) << "Length" << "\t" << setw(10) << "iNode" << "\tPathname\n";
    out << setw(10) << "------" << "\t" << setw(10) << "-----" << "\t--------\n";

    for (const FileID& f : fileList) {
        out << setw(10) << f.getLength()
            << "\t" << setw(10) << f.getInode()
            << "\t" << f.getPath() << "\n";
    }
    out << "--- End of File List ---\n";
}
