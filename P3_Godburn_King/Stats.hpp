// -------------------------------------------------
// Stats.hpp
// Declaration of the Stats adapter class.
// Derived from C struct stat to provide an
// object-oriented interface for file metadata.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#ifndef STATS_HPP
#define STATS_HPP

#include <sys/stat.h>
#include <iostream>
#include <string>

using namespace std;

// -------------------------------------------------
// Stats: Public adapter class derived from the
// C struct stat.  Inherits all stat fields and
// adds a print function.
// Fields used: st_ino, st_nlink, st_uid, st_gid,
//   st_size, st_mtimespec/st_mtim, st_ctimespec.
// Fields ignored: st_dev, st_mode, st_rdev,
//   st_atimespec, st_blocks, st_blksize,
//   st_flags, st_gen.
// -------------------------------------------------
class Stats : public stat {
public:
    // Default constructor.
    Stats() = default;

    // Default destructor (no dynamic allocation).
    ~Stats() = default;

    // Print the relevant stat fields to the
    // given output stream.
    void print(ostream& out) const;
};

#endif
