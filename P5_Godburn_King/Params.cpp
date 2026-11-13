// -------------------------------------------------
// Params.cpp
// Implementation of the Params class for sweeper.
// Uses getopt_long() to parse command-line args.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "Params.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include <climits>
#include <cstring>

using namespace std;

// -------------------------------------------------
// Constructor: parse the command line with
// getopt_long() and initialize all data members.
// Parameters:
//   argc - argument count from main
//   argv - argument vector from main
// -------------------------------------------------
Params::Params(int argc, char* argv[])
    : startDir("."),
      outFileName(""),
      verboseFlag(false),
      helpFlag(false),
      ignoreCaseFlag(false),
      minFileSize(1) {

    // Define the long options table.
    // Some long opts have short synonyms,
    // some do not.
    // Short-only: -d (starting directory)
    // Long-only:  --size, --ignore-case
    // Both:       -v/--verbose, -h/--help,
    //             -o/--output
    static struct option longOptions[] = {
        {"verbose",     no_argument,       
             nullptr, 'v'},
        {"help",        no_argument,       
             nullptr, 'h'},
        {"output",      required_argument, 
             nullptr, 'o'},
        {"size",        required_argument, 
             nullptr, 'S'},
        {"ignore-case", no_argument,       
             nullptr, 'I'},
        {nullptr,       0,                 
             nullptr,  0 }
    };

    // Short options string for getopt_long.
    // -v  verbose
    // -h  help
    // -o: output file (requires argument)
    // -d: starting directory (requires argument)
    // Note: 'S' and 'I' are internal codes for
    // long-only options (--size, --ignore-case).
    const char* shortOptions = "vho:d:";

    int option;
    while ((option = getopt_long(argc, argv,
            shortOptions, longOptions, 
            nullptr)) != -1) {

        switch (option) {
        case 'v':
            verboseFlag = true;
            break;

        case 'h':
            helpFlag = true;
            break;

        case 'o':
            outFileName = string(optarg);
            break;

        case 'd':
            startDir = string(optarg);
            break;

        // Long-only: --size <n>
        case 'S':
            minFileSize = atol(optarg);
            if (minFileSize < 1) {
                cerr << "Error: size must be"
                     << " at least 1."
                     << endl;
                exit(1);
            }
            break;

        // Long-only: --ignore-case
        case 'I':
            ignoreCaseFlag = true;
            break;

        case '?':
            // getopt_long prints an error.
            usage();
            exit(1);
            break;

        default:
            break;
        }
    }

    // If help was requested, print usage and
    // exit normally.
    if (helpFlag) {
        usage();
        exit(0);
    }

    // Resolve the starting directory to a full
    // path. First chdir to it, then use getcwd.
    if (chdir(startDir.c_str()) != 0) {
        cerr << "Error: cannot access "
             << "directory: "
             << startDir << endl;
        exit(1);
    }

    // Get the full pathname after chdir.
    char fullPath[PATH_MAX];
    if (getcwd(fullPath, 
               sizeof(fullPath)) != nullptr) {
        startDir = string(fullPath);
    } else {
        cerr << "Error: getcwd failed."
             << endl;
        exit(1);
    }

    // Open the output file if one was given.
    // Otherwise output goes to cout.
    if (!outFileName.empty()) {
        outStream.open(outFileName);
        if (!outStream.is_open()) {
            cerr << "Error: cannot open "
                 << "output file: "
                 << outFileName << endl;
            exit(1);
        }
    }
}

// -------------------------------------------------
// Destructor: close the output stream if open.
// -------------------------------------------------
Params::~Params() {
    if (outStream.is_open()) {
        outStream.close();
    }
}

// -------------------------------------------------
// usage(): Print a usage message describing
// the sweeper command and all legal switches.
// -------------------------------------------------
void Params::usage() {
    cout
    << "Usage: sweeper [options]" << endl
    << endl
    << "Options:" << endl
    << "  -v, --verbose       "
    << "Print names of all files and" << endl
    << "                      "
    << "directories opened." << endl
    << "  -h, --help          "
    << "Display this help message." << endl
    << "  -o, --output <file> "
    << "Write output to the named" << endl
    << "                      "
    << "file." << endl
    << "  -d <path>           "
    << "Start searching in the" << endl
    << "                      "
    << "given directory." << endl
    << "                      "
    << "(Default: current directory)"
    << endl
    << "      --size <n>      "
    << "Only process files of at" << endl
    << "                      "
    << "least n bytes." << endl
    << "      --ignore-case   "
    << "Ignore case when comparing"
    << endl
    << "                      "
    << "file names." << endl
    << endl;
}

// -------------------------------------------------
// print(): Display all data members neatly
// to the output stream (file or cout).
// -------------------------------------------------
void Params::print() {
    // Choose the output destination.
    ostream& dest = outStream.is_open()
                    ? (ostream&) outStream
                    : cout;

    dest << "------- Sweeper Settings "
         << "-------" << endl;

    dest << "Start at:     "
         << startDir << endl;

    dest << "Output file:  "
         << (outFileName.empty() 
             ? "(stdout)" : outFileName)
         << endl;

    dest << "Verbose?      "
         << (verboseFlag ? "Yes" : "No")
         << endl;

    dest << "Ignore case?  "
         << (ignoreCaseFlag ? "Yes" : "No")
         << endl;

    dest << "Min file size: "
         << minFileSize << endl;

    dest << "----------------------------"
         << endl;
}
