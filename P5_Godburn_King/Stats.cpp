// -------------------------------------------------
// Stats.cpp
// Implementation of the Stats adapter class.
// Formats and prints the useful fields of the
// inherited struct stat.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "Stats.hpp"
#include <iomanip>
#include <ctime>

using namespace std;

// -------------------------------------------------
// print(): Format and output the relevant fields
// of the stat structure.
// Parameters:
//   out - the output stream to write to.
// -------------------------------------------------
void Stats::print(ostream& out) const {
    out << "  iNode:   " << st_ino   << endl;
    out << "  Links:   " << st_nlink << endl;
    out << "  UID:     " << st_uid   << endl;
    out << "  GID:     " << st_gid   << endl;
    out << "  Size:    " << st_size
        << " bytes" << endl;

    // Format the last-modified time.
    // On Linux the field is st_mtim;
    // on Darwin it is st_mtimespec.
#ifdef __APPLE__
    time_t modTime = st_mtimespec.tv_sec;
    time_t chgTime = st_ctimespec.tv_sec;
#else
    time_t modTime = st_mtim.tv_sec;
    time_t chgTime = st_ctim.tv_sec;
#endif
    out << "  Modified: "
        << ctime(&modTime);
    out << "  Changed:  "
        << ctime(&chgTime);
}
