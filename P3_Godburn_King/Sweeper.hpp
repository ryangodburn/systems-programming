// -------------------------------------------------
// Sweeper.hpp
// Declaration of the Sweeper controller class.
// Manages the directory scanning process for
// the sweeper duplicate-file finder.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#ifndef SWEEPER_HPP
#define SWEEPER_HPP

#include "Params.hpp"
#include "Direntry.hpp"
#include "Stats.hpp"
#include "FileID.hpp"
#include <vector>
#include <string>

using namespace std;

// -------------------------------------------------
// Sweeper: Controller class that opens a single
// directory, reads its entries, gathers file
// stats, and stores FileID records for each
// regular file found.
// -------------------------------------------------
class Sweeper {
private:
    // Reference to the Params object.
    Params& params;

    // Simple name of the starting directory.
    string startName;

    // Full pathname of the directory to process.
    string fullPath;

    // Pointer to the current directory entry
    // returned by readdir(), cast to Direntry*.
    Direntry* currentEntry;

    // Stats object filled by lstat() for the
    // current file being examined.
    Stats currentStats;

    // Temporary FileID used to build each record
    // before pushing it onto the vector.
    FileID tempFile;

    // Vector holding FileID info for every
    // regular file found in the directory.
    vector<FileID> fileList;

public:
    // Constructor: initialize from the Params
    // object; extract starting directory info.
    Sweeper(Params& par);

    // Destructor.
    ~Sweeper() = default;

    // Process one directory: open it, read
    // entries, gather stats, and build the
    // file list.
    void oneDir();

    // Print the collected file list.
    void printFileList(ostream& out) const;
};

#endif
