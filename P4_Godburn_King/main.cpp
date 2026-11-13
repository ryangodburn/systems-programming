// -------------------------------------------------
// main.cpp
// Entry point for the sweeper utility, Program 4.
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
    cout << "Welcome to P4: Recursive Sweeper"
         << endl;

    // Parse command-line arguments.
    Params settings(argc, argv);
    settings.print();

    Sweeper S(settings);

    // P4: start with absolute pathname from Params and call travel()
    const string& startAbs = settings.getStartDir();
    S.travel(startAbs);

    // P4: sort by length and print results
    S.sortByLength();
    S.printFileList(settings.getOut());    

    // Normal exit message.
    bye();
    return 0;
}
