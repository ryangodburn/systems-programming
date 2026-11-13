// -------------------------------------------------
// FileID.hpp
// Declaration of the FileID class.
// Stores identifying information about one file:
// name, inode, size, and absolute pathname.
// Includes operator< for iNode-based sorting.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#ifndef FILEID_HPP
#define FILEID_HPP

#include <sys/types.h>
#include <iostream>
#include <string>

using namespace std;

// -------------------------------------------------
// FileID: Holds data about a single regular file
// found during a directory scan.
// Data comes from both the directory entry and
// the inode (via lstat).
// -------------------------------------------------
class FileID {
private:
    // Simple file name from the directory entry.
    string fileName;

    // iNode number from the directory entry.
    ino_t inodeNumber;

    // File length in bytes from the iNode.
    off_t fileLength;

    // Full absolute pathname of the file.
    string absPath;

public:
    // Constructor: initialize from name, path,
    // inode number, and file size.
    FileID(const string& name,
           const string& path,
           ino_t inode, off_t length);

    // Default destructor (no dynamic allocation).
    ~FileID() = default;

    // Print the inode number and absolute
    // pathname in formatted columns.
    void print(ostream& out) const;

    // -----------------------------------------
    // Member operator< for iNode-based sorting.
    // Compares two FileID objects by their
    // inode numbers.  Used by std::sort when
    // no custom comparator is provided.
    // -----------------------------------------
    bool operator<(const FileID& other) const;

    // --- Inline accessors --------------------

    // Return the simple file name.
    const string& getFileName() const {
        return fileName;
    }

    // Return the inode number.
    ino_t getInode() const {
        return inodeNumber;
    }

    // Return the file size in bytes.
    off_t getLength() const {
        return fileLength;
    }

    // Return the absolute pathname.
    const string& getPath() const {
        return absPath;
    }
};

// -------------------------------------------------
// Global comparison function for sorting FileID
// objects by file length.  Passed as a custom
// comparator to std::sort for the length-based
// duplicate search.
// -------------------------------------------------
bool compareByLength(
    const FileID& left,
    const FileID& right);

#endif
