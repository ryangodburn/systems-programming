// kid.cpp -------------------------------------------
// Authors: Ryan Godburn and Emily King
// Musical Chairs with Sockets - Kid (client).
// The kid connects to mom's server, then hands
// control to KidLogic to play the game.
// Command line: kid <hostname> <kidname>
// ---------------------------------------------------
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "socktypes.h"
#include "kidLogic.hpp"
#include "protocol.hpp"

using namespace std;

// ---------------------------------------------------
// main: create a socket, connect to mom, then
// run the kid logic. When run() returns, close
// the socket and terminate.
// ---------------------------------------------------
int main(int argc, char* argv[]) {
    // ---- Validate command line ----
    if (argc != 3) {
        cerr << "usage: " << argv[0]
             << " hostname kidname\n";
        return 1;
    }

    const char* hostName = argv[1];
    string kidName = argv[2];

    // ---- Create socket ----
    int momFd = socket(AF_INET, SOCK_STREAM, 0);
    if (momFd < 0) {
        perror("socket");
        return 1;
    }

    // ---- Look up host ----
    hostent* host = gethostbyname(hostName);
    if (host == nullptr) {
        cerr << "Could not find host: "
             << hostName << endl;
        close(momFd);
        return 1;
    }

    // ---- Build server address ----
    SockAddrIn momAddr;
    memset(&momAddr, 0, sizeof(momAddr));
    momAddr.sin_family = AF_INET;
    momAddr.sin_port   = htons(PORTNUM);
    memcpy(&momAddr.sin_addr,
           host->h_addr_list[0],
           host->h_length);

    // ---- Connect to mom ----
    if (connect(momFd,
                (SockAddr*)&momAddr,
                sizeof(momAddr)) < 0) {
        perror("connect");
        close(momFd);
        return 1;
    }

    cout << kidName << ": connected to mom at "
         << hostName << " on port "
         << PORTNUM << endl;

    // ---- Play the game ----
    // KidLogic opens streams on momFd.
    // Its destructor closes them (and the fd).
    {
        KidLogic kid(momFd, kidName);
        kid.run();
    }
    // KidLogic destructor runs here, closing
    // momIn (which closes momFd) and momOut.

    return 0;
}
