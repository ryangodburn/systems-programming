// -------------------------------------------------
// main.cpp
// Entry point for the sweeper utility.
// Creates a Params object from the command line,
// then prints the parsed settings.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "Params.hpp"
#include <iostream>

using namespace std;

// -------------------------------------------------
// main(): Accept command-line arguments, build a
// Params object to parse them, and print results.
// -------------------------------------------------
int main(int argc, char* argv[]) {
    // Construct Params, which parses the
    // command line via getopt_long().
    Params settings(argc, argv);

    // Display all parsed settings.
    settings.print();

    return 0;
}
