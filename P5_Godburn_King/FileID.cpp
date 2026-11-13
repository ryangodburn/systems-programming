// -------------------------------------------------
// FileID.cpp
// Implementation of the FileID class.
// Stores and prints identifying info for one
// file. Includes operator< for iNode sorting
// and a global comparator for length sorting.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "FileID.hpp"
#include <iomanip>

using namespace std;

// -------------------------------------------------
// Constructor: build a FileID from the given
// file name, directory path, inode, and size.
// The absolute pathname is constructed by
// appending the file name to the directory path.
// Parameters:
//   name   - simple file name from dir entry.
//   path   - absolute path of the directory.
//   inode  - inode number from dir entry.
//   length - file size in bytes from lstat.
// -------------------------------------------------
FileID::FileID(
    const string& name,
    const string& path,
    ino_t inode,
    off_t length)
    : fileName(name),
      inodeNumber(inode),
      fileLength(length) {
    // Build the absolute pathname by joining
    // the directory path and the file name.
    absPath = path + "/" + name;
}

// -------------------------------------------------
// print(): Output the inode number and absolute
// pathname in neat tab-separated columns.
// Parameters:
//   out - the output stream to write to.
// -------------------------------------------------
void FileID::print(ostream& out) const {
    out << setw(10) << inodeNumber
        << "\t" << absPath << endl;
}

// -------------------------------------------------
// operator<(): Member comparison for iNode-
// based sorting.  Compares two FileID objects
// by their inode number in ascending order.
// Ties are broken by pathname for stable output.
// Parameters:
//   other - the right-hand FileID to compare.
// Returns: true if this iNode < other's iNode.
// -------------------------------------------------
bool FileID::operator<(
    const FileID& other) const {
    if (inodeNumber != other.inodeNumber)
        return inodeNumber < other.inodeNumber;
    return absPath < other.absPath;
}

// -------------------------------------------------
// compareByLength(): Global comparison function
// for sorting FileID objects by file length.
// This is passed as a custom comparator to
// std::sort for the length-based duplicate
// search.  Ties broken by pathname.
// Parameters:
//   left  - first FileID to compare.
//   right - second FileID to compare.
// Returns: true if left's length < right's.
// -------------------------------------------------
bool compareByLength(
    const FileID& left,
    const FileID& right) {
    if (left.getLength() != right.getLength())
        return left.getLength()
             < right.getLength();
    return left.getPath() < right.getPath();
}
