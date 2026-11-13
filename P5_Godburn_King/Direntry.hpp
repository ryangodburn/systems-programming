// -------------------------------------------------
// Direntry.hpp
// Declaration of the Direntry adapter class.
// Derived privately from C struct dirent to
// provide controlled access to directory entries.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#ifndef DIRENTRY_HPP
#define DIRENTRY_HPP

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <string>

using namespace std;

// -------------------------------------------------
// Direntry: Private adapter class derived from
// the C struct dirent.  Provides accessors and
// a print function for directory entry data.
//
// Fields used: d_ino, d_type, d_name.
// -------------------------------------------------
class Direntry : private dirent {
public:
    // Default constructor.
    Direntry() = default;

    // Default destructor (no dynamic allocation).
    ~Direntry() = default;

    // Print the file type, inode number, and
    // name in formatted columns.
    void print(ostream& out) const;

    // --- Inline accessors --------------------

    // Return the inode number.
    ino_t getInode() const {
        return d_ino;
    }

    // Return the file type code.
    unsigned char getType() const {
        return d_type;
    }

    // Return the entry name as a C++ string.
    string getName() const {
        return string(d_name);
    }
};

#endif
