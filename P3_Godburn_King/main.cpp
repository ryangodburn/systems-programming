// -------------------------------------------------
// main.cpp
// Entry point for the sweeper utility, Program 3.
// Creates Params and Sweeper objects, then
// processes one directory to list its files.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "tools.hpp"
#include "Params.hpp"
#include "Sweeper.hpp"
#include <unistd.h>

using namespace std;

// -------------------------------------------------
// main(): Accept command-line arguments, build
// Params and Sweeper objects, chdir into the
// starting directory, and process it.
// -------------------------------------------------
int main(int argc, char* argv[]) {
    // Identify the program and print the date.
    banner();
    cout << "Welcome to P3: Sweeper"
         << endl;

    // Parse command-line arguments.
    Params settings(argc, argv);
    settings.print();

    // Build the Sweeper controller from the
    // parsed parameters.
    Sweeper sweeper(settings);

    // Change into the starting directory.
    const string& startDir =
        settings.getStartDir();

    if (chdir(startDir.c_str()) != 0) {
        fatal("Cannot chdir to: "
              + startDir);
    }

    // Process the single directory.
    sweeper.oneDir();

    // Normal exit message.
    bye();
    return 0;
}
