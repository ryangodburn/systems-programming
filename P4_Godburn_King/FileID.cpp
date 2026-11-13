// -------------------------------------------------
// FileID.cpp
// Implementation of the FileID class.
// Stores and prints identifying info for one file.
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
