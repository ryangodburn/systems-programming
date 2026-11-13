/**
 * Program 1: Hello Shell
 * CSCI 4547 Systems Programming - Spring 2026
 * 
 * Authors: Emily King & Ryan Godburn
 * 
 * Description:
 *   This program demonstrates command-line argument processing
 *   in C++. It reads arguments passed from the shell and
 *   categorizes each one as:
 *     - command:  The executable name (argv[0])
 *     - switch:   Arguments beginning with a dash (-)
 *     - argument: All other arguments
 * 
 *   Output is sent to both the console (cout) and an output
 *   file in append mode, allowing multiple test runs to
 *   accumulate in a single file.
 * 
 * Compile:
 *   Linux:  g++ -std=c++23 -Wall -o P1_King_Godburn main.cpp
 *   Mac:    clang++ -std=c++23 -Wall -o P1_King_Godburn main.cpp
 * 
 * Usage:
 *   ./P1_King_Godburn [switches] [arguments]
 */
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
    
    // Open output file in append mode so multiple runs accumulate
    // File naming convention: P1_LastName.txt
    ofstream myout("P1_King_Godburn.txt", ios::out | ios::app);
    
    // Verify the file opened successfully
    if (!myout) {
        cerr << "Error opening output file." << endl;
        return 1;
    }
    
    // Print a dashed divider line to separate test runs
    cout << "-----------------------------------" << endl;
    myout << "-----------------------------------" << endl;

    // Process each command-line argument
    for (int k = 0; k < argc; k++) {
        
        if (k == 0) {
            // argv[0] is always the command/executable name
            cout << "command " << argv[k] << endl;
            myout << "command " << argv[k] << endl;
        }
        else if (argv[k][0] == '-') {
            // Arguments starting with '-' are switches/options
            // argv[k]+1 skips the leading dash
            cout << "switch " << (argv[k] + 1) << endl;
            myout << "switch " << (argv[k] + 1) << endl;
        }
        else {
            // All other arguments (file names, paths, etc.)
            cout << "argument " << argv[k] << endl;
            myout << "argument " << argv[k] << endl;
        }
    }
    
    // Close the output file
    myout.close();
    
    return 0;
}