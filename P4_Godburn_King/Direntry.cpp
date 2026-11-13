// -------------------------------------------------
// Direntry.cpp
// Implementation of the Direntry adapter class.
// Provides formatted output for directory entries.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "Direntry.hpp"
#include <iomanip>

using namespace std;

// -------------------------------------------------
// Helper: convert a d_type code to a readable
// string label for output.
// -------------------------------------------------
static string typeName(unsigned char typeCode) {
    switch (typeCode) {
    case DT_REG:  return "REG ";
    case DT_DIR:  return "DIR ";
    case DT_LNK:  return "LINK";
    case DT_FIFO: return "FIFO";
    case DT_SOCK: return "SOCK";
    case DT_CHR:  return "CHR ";
    case DT_BLK:  return "BLK ";
    default:      return "??? ";
    }
}

// -------------------------------------------------
// print(): Format and output the file type, inode
// number, and entry name in neat columns.
// The variable-length name is placed on the
// right end of the line.
// Parameters:
//   out - the output stream to write to.
// -------------------------------------------------
void Direntry::print(ostream& out) const {
    out << "  " << typeName(d_type)
        << setw(10) << d_ino
        << "  " << d_name << endl;
}
