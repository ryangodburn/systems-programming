// -------------------------------------------------
// main.cpp
// Entry point for the sweeper utility, Program 5.
// Creates Params and Sweeper objects, traverses
// the directory tree, and identifies duplicate
// files by iNode and by content comparison.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#include "tools.hpp"
#include "Params.hpp"
#include "Sweeper.hpp"
#include <unistd.h>

using namespace std;

// -------------------------------------------------
// main(): Accept command-line arguments, build
// Params and Sweeper objects, traverse the
// directory tree, and find duplicate files.
// -------------------------------------------------
int main(int argc, char* argv[]) {
    // Identify the program and print the date.
    banner();
    cout << "Welcome to P5: "
         << "Eliminating Duplicates"
         << endl;

    // Parse command-line arguments.
    Params settings(argc, argv);
    settings.print();

    // Create the Sweeper controller.
    Sweeper sweep(settings);

    // Traverse the directory tree starting
    // from the absolute path in settings.
    const string& startAbs =
        settings.getStartDir();
    sweep.travel(startAbs);

    // Sort by length and print all files
    // found during traversal.
    sweep.sortByLength();
    sweep.printFileList(settings.getOut());

    // Call findDups to identify duplicate
    // files.  It receives the file list by
    // reference and sorts it in two passes:
    // first by iNode, then by length.
    sweep.findDups(sweep.getFileList());

    // Normal exit message.
    bye();
    return 0;
}
