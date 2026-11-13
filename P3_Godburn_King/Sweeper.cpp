// -------------------------------------------------
// Sweeper.cpp
// Implementation of the Sweeper controller class.
// Opens one directory, reads entries, gathers
// stats with lstat(), and builds a list of
// FileID records for all regular files.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "Sweeper.hpp"
#include "tools.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <sstream>
#include <iomanip>
#include <cstring>

using namespace std;

// -------------------------------------------------
// Constructor: extract the starting directory
// name and full pathname from the Params object.
// Parameters:
//   par - reference to the initialized Params.
// -------------------------------------------------
Sweeper::Sweeper(Params& par)
    : params(par),
      currentEntry(nullptr),
      tempFile("", "", 0, 0) {

    // Get the full path from Params.
    fullPath = params.getStartDir();

    // Extract the simple directory name from
    // the end of the full path.
    // Wrap in istringstream to parse the path.
    size_t lastSlash = fullPath.rfind('/');
    if (lastSlash != string::npos) {
        startName =
            fullPath.substr(lastSlash + 1);
    } else {
        startName = fullPath;
    }
}

// -------------------------------------------------
// oneDir(): Process the current working directory.
// Opens it, reads each entry, filters by type,
// calls lstat() on regular files, and pushes
// FileID records onto the file list vector.
// -------------------------------------------------
void Sweeper::oneDir() {
    ostream& out = params.getOut();

    // Open the current working directory.
    DIR* dirPtr = opendir(".");
    if (dirPtr == nullptr) {
        cerr << "Error: cannot open directory: "
             << fullPath << endl;
        return;
    }

    // Print the directory being processed.
    out << "\nProcessing directory: "
        << fullPath << endl;

    // Read entries one at a time.
    struct dirent* rawEntry;
    while ((rawEntry = readdir(dirPtr))
            != nullptr) {

        // Cast the raw dirent* to Direntry*.
        currentEntry =
            reinterpret_cast<Direntry*>(
                rawEntry);

        unsigned char entryType =
            currentEntry->getType();
        string entryName =
            currentEntry->getName();

        // Skip the dot and dot-dot entries.
        // Check by name rather than assuming
        // they are the first two entries,
        // since directory order varies by OS.
        if (entryName == "."
            || entryName == "..") {
            continue;
        }

        // Skip anything that is not a regular
        // file or a directory.  This discards
        // soft links, pipes, sockets, devices.
        if (entryType != DT_REG
            && entryType != DT_DIR) {
            continue;
        }

        // If verbose mode is on, print the
        // name of each file or directory found.
        if (params.isVerbose()) {
            currentEntry->print(out);
        }

        // If it is a directory, do nothing
        // more for now.  (P4 will recurse.)
        if (entryType == DT_DIR) {
            continue;
        }

        // It is a regular file (DT_REG).
        // Call lstat() to get the file stats.
        // Stats derives from struct stat, so
        // we can pass its address to lstat().
        int statResult =
            lstat(entryName.c_str(),
                  &currentStats);

        if (statResult != 0) {
            cerr << "Warning: lstat failed"
                 << " for: " << entryName
                 << endl;
            continue;
        }

        // Build the absolute path for this
        // file by joining directory path
        // and the simple file name.
        string filePath =
            fullPath + "/" + entryName;

        // Skip files smaller than the minimum
        // size threshold from the Params.
        if (currentStats.st_size
            < params.getMinFileSize()) {
            continue;
        }

        // Create a FileID and push it onto
        // the file list vector.
        tempFile = FileID(
            entryName,
            fullPath,
            currentEntry->getInode(),
            currentStats.st_size);

        fileList.push_back(tempFile);
    }

    // Close the directory.
    closedir(dirPtr);

    out << "Done processing: "
        << fullPath << endl;

    // Print all files found in this directory.
    printFileList(out);
}

// -------------------------------------------------
// printFileList(): Output the collected file list
// showing inode and absolute path for each file.
// Parameters:
//   out - the output stream to write to.
// -------------------------------------------------
void Sweeper::printFileList(
    ostream& out) const {

    out << "\n--- File List ("
        << fileList.size()
        << " files) ---" << endl;

    out << setw(10) << "iNode"
        << "\tPathname" << endl;

    out << setw(10) << "-----"
        << "\t--------" << endl;

    for (const FileID& file : fileList) {
        file.print(out);
    }

    out << "--- End of File List ---"
        << endl;
}
