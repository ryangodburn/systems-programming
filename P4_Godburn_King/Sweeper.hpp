// -------------------------------------------------
// Sweeper.hpp
// Declaration of the Sweeper controller class.
// Manages the directory scanning process for
// the sweeper duplicate-file finder.
// By Ryan Godburn and Emily King
// -------------------------------------------------
#ifndef SWEEPER_HPP
#define SWEEPER_HPP

#include "Params.hpp"
#include "Direntry.hpp"
#include "Stats.hpp"
#include "FileID.hpp"
#include <vector>
#include <string>
#include <ostream> 

using namespace std;

// -------------------------------------------------
// Sweeper: Controller class that opens a single
// directory, reads its entries, gathers file
// stats, and stores FileID records for each
// regular file found.
// -------------------------------------------------
class Sweeper {
private:
    // Reference to the Params object.
    Params& params;
    
    // vector holding FileID info for every regulat file found
    vector<FileID> fileList;
    
    // helpers 
    static string joinPath(const string& base, const string& name);

public:
  explicit Sweeper(Params& par);
  ~Sweeper() = default;
  
  // recursively traverse directory tree starting at absPath
  void travel(const string& absPath);
  
  // sort by file length
  void sortByLength();
  
  //print collected list
  void printFileList(ostream& out) const;
};

#endif
