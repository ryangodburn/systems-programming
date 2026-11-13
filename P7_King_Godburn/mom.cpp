// mom.cpp -------------------------------------------
// Authors: Ryan Godburn and Emily King
// Musical Chairs with Sockets - Mom (server).
// Mom sets up a welcome socket, accepts N kid
// connections, then hands control to MomLogic
// for the game rounds.
// ---------------------------------------------------
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "socktypes.h"
#include "momLogic.hpp"
#include "protocol.hpp"

using namespace std;

// ---------------------------------------------------
// main: set up the welcome socket, accept kids,
// close the welcome socket, then run the game.
// Command line: mom <numKids>
// ---------------------------------------------------
int main(int argc, char* argv[]) {
    // ---- Validate command line ----
    if (argc != 2) {
        cerr << "usage: " << argv[0]
             << " numKids\n";
        return 1;
    }

    int nKids = atoi(argv[1]);

    if (nKids <= 1 || nKids > MAX_KIDS) {
        cerr << "Number of kids must be between 2"
             << " and " << MAX_KIDS << endl;
        return 1;
    }

    // ---- Create welcome socket ----
    int welcomeSock =
        socket(AF_INET, SOCK_STREAM, 0);
    if (welcomeSock < 0) {
        perror("socket");
        return 1;
    }

    // Allow port reuse so we can restart quickly
    // after an abnormal termination.
    int trueVal = 1;
    if (setsockopt(welcomeSock, SOL_SOCKET,
                   SO_REUSEADDR,
                   &trueVal, sizeof(int)) < 0) {
        perror("setsockopt");
        return 1;
    }

    // ---- Bind to the protocol port ----
    SockAddrIn addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORTNUM);

    if (bind(welcomeSock,
             (SockAddr*)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // ---- Listen with a generous queue ----
    // Queue > 1 so multiple kids arriving at
    // once are not refused.
    if (listen(welcomeSock, 10) < 0) {
        perror("listen");
        return 1;
    }

    cout << "Mom: waiting for " << nKids
         << " kids on port " << PORTNUM
         << "...\n";

    // ---- Accept all kid connections ----
    pollfd* socks = new pollfd[nKids];

    for (int i = 0; i < nKids; i++) {
        SockAddrIn kidAddr;
        socklen_t kidLen = sizeof(kidAddr);

        int fd = accept(welcomeSock,
                        (SockAddr*)&kidAddr,
                        &kidLen);
        if (fd < 0) {
            perror("accept");
            return 1;
        }

        socks[i].fd      = fd;
        socks[i].events  = POLLIN;
        socks[i].revents = 0;

        cout << "Mom: kid " << i
             << " connected\n";
    }

    // ---- Close welcome socket ----
    // No more kids will be accepted.
    close(welcomeSock);

    // ---- Run the game ----
    MomLogic mom(nKids, socks);
    mom.run();

    // ---- Clean up ----
    delete[] socks;

    cout << "Mom: game over\n";
    return 0;
}
