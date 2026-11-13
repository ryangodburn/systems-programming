// -------------------------------------------------
// Params.hpp
// Declaration of the Params class for sweeper.
// Parses and stores command-line options using
// getopt_long().
// By Ryan Godburn and Emily King
// -------------------------------------------------
#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <fstream>

using namespace std;

// -------------------------------------------------
// The Params class stores all command-line settings
// for the sweeper duplicate-file finder utility.
// -------------------------------------------------
class Params {
private:
    // Full pathname of the starting directory.
    string startDir;

    // Relative pathname of the output file.
    string outFileName;

    // Open output stream for writing results.
    ofstream outStream;

    // Boolean flags for each switch.
    bool verboseFlag;
    bool helpFlag;
    bool ignoreCaseFlag;

    // Minimum file size to process (default 1).
    long minFileSize;

public:
    // Constructor: parse argc/argv with
    // getopt_long() and initialize all members.
    Params(int argc, char* argv[]);

    // Destructor: close the output stream.
    ~Params();

    // Print a usage message explaining how to
    // invoke sweeper and listing all switches.
    void usage();

    // Print all data members (except the open
    // ofstream) to the output stream.
    void print();
};

#endif
