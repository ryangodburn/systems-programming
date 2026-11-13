// -------------------------------------------------
// Sweeper.hpp
// Declaration of the Sweeper controller class.
// Manages the directory scanning and duplicate
// file identification for Program 5.
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
#include <ostream>

using namespace std;

// -------------------------------------------------
// Sweeper: Controller class that opens a single
// directory, reads its entries, gathers file
// stats, stores FileID records, and identifies
// duplicate files using iNode and length
// comparisons.
// -------------------------------------------------
class Sweeper {
private:
    // Reference to the Params object.
    Params& params;

    // Vector holding FileID info for every
    // regular file found during traversal.
    vector<FileID> fileList;

    // Helper to join a base path and name.
    static string joinPath(
        const string& base,
        const string& name);

    // Helper: compare two files byte-by-byte.
    // Returns true if their contents match.
    static bool filesMatch(
        const string& pathA,
        const string& pathB);

public:
    // Constructor: store Params reference.
    explicit Sweeper(Params& par);

    // Default destructor.
    ~Sweeper() = default;

    // Recursively traverse a directory tree
    // starting at absPath.
    void travel(const string& absPath);

    // Sort by file length using the global
    // compareByLength comparator.
    void sortByLength();

    // Print the collected file list.
    void printFileList(ostream& out) const;

    // Return a mutable reference to the
    // internal file list vector.
    vector<FileID>& getFileList() {
        return fileList;
    }

    // -----------------------------------------
    // findDups(): Identify duplicate files in
    // the fileList vector.  Sorts by iNode to
    // find hard links, then sorts by length
    // to find content-identical copies.
    // Prints all duplicates to the output.
    // Parameter:
    //   fileVec - vector of FileIDs to scan.
    // -----------------------------------------
    void findDups(vector<FileID>& fileVec);
};

#endif
