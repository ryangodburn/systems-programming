// momLogic.cpp --------------------------------------
// Authors: Ryan Godburn and Emily King
// MomLogic class implementation for Musical Chairs
// with Sockets. Runs the game rounds, manages
// the chair array, and communicates with kids
// using the protocol messages.
// ---------------------------------------------------
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cstdarg>
#include <poll.h>

#include "momLogic.hpp"
#include "protocol.hpp"

using namespace std;

// ---------------------------------------------------
// Constructor: allocate the chair string and
// the player array. The polling array is owned
// by mom.cpp and passed in by pointer.
// ---------------------------------------------------
MomLogic::MomLogic(int nk, pollfd sockArray[]) {
    nKids  = nk;
    nAlive = nk;
    socks  = sockArray;

    chairs  = new char[nKids];   // max possible size
    players = new Player[nKids];
}

// ---------------------------------------------------
// Destructor: free dynamic memory and close any
// FILE* streams that are still open.
// ---------------------------------------------------
MomLogic::~MomLogic() {
    for (int k = 0; k < nKids; k++) {
        closePlayer(k);
    }
    delete[] chairs;
    delete[] players;
}

// ---------------------------------------------------
// closePlayer: safely close the FILE* streams
// for player k. Sets pointers to nullptr so we
// never double-close. fclose also closes the
// underlying file descriptor.
// ---------------------------------------------------
void MomLogic::closePlayer(int k) {
    if (players[k].kidIn) {
        fclose(players[k].kidIn);
        players[k].kidIn = nullptr;
    }
    if (players[k].kidOut) {
        fclose(players[k].kidOut);
        players[k].kidOut = nullptr;
    }
}

// ---------------------------------------------------
// countAlive: return the number of kids whose
// alive flag is still true.
// ---------------------------------------------------
int MomLogic::countAlive() const {
    int count = 0;
    for (int i = 0; i < nKids; i++) {
        if (players[i].alive) count++;
    }
    return count;
}

// ---------------------------------------------------
// findWinner: return the index of the single
// surviving kid, or -1 if none found.
// ---------------------------------------------------
int MomLogic::findWinner() const {
    for (int i = 0; i < nKids; i++) {
        if (players[i].alive) return i;
    }
    return -1;
}

// ---------------------------------------------------
// initPlayers: read one HELLO message from each
// kid and store the name. Open input and output
// streams on each kid's socket using fdopen.
// The output stream uses dup() so that closing
// one stream does not close the other's fd.
// ---------------------------------------------------
void MomLogic::initPlayers() {
    char cmd[32], name[128];

    for (int k = 0; k < nKids; k++) {
        players[k].alive = true;

        // Open two C-streams on the socket.
        // dup() gives the output stream its own
        // fd so fclose won't conflict.
        players[k].kidIn =
            fdopen(socks[k].fd, "r");
        players[k].kidOut =
            fdopen(dup(socks[k].fd), "w");

        if (!players[k].kidIn ||
            !players[k].kidOut) {
            perror("fdopen");
            exit(1);
        }

        // Read the HELLO message from this kid.
        if (fscanf(players[k].kidIn,
                   "%31s %127s", cmd, name) != 2) {
            perror("reading HELLO");
            exit(1);
        }

        players[k].name = name;

        printf("Mom: kid %d says HELLO, name=%s\n",
               k, name);
    }
}

// ---------------------------------------------------
// initRound: prepare the chairs and notify all
// living kids that a new round is starting.
// The chair array is shortened to nAlive-1 and
// every chair is set to '1' (available).
// ---------------------------------------------------
void MomLogic::initRound() {
    int chairCount = nAlive - 1;

    // Initialize chair string.
    for (int i = 0; i < chairCount; i++) {
        chairs[i] = '1';
    }
    chairs[chairCount] = '\0';

    printf("\n=== New Round: %d kids, %d chairs ===\n",
           nAlive, chairCount);

    // Send GETUP with the chair count to each
    // living kid.
    for (int k = 0; k < nKids; k++) {
        if (!players[k].alive) continue;

        fprintf(players[k].kidOut,
                "GETUP %d\n", chairCount);
        fflush(players[k].kidOut);

        printf("Mom -> %s: GETUP %d\n",
               players[k].name.c_str(), chairCount);
    }
}

// ---------------------------------------------------
// stopTheMusic: sleep briefly so kids can "march",
// then send SIT to all living kids. Enter a
// polling loop to handle WANT requests until
// one kid is eliminated.
// ---------------------------------------------------
void MomLogic::stopTheMusic() {
    sleep(1);

    // Send SIT to every living kid.
    for (int k = 0; k < nKids; k++) {
        if (!players[k].alive) continue;

        fprintf(players[k].kidOut, "SIT\n");
        fflush(players[k].kidOut);

        printf("Mom -> %s: SIT\n",
               players[k].name.c_str());
    }

    // Process WANT messages until one kid
    // is left standing (chairsLeft goes to -1).
    int chairsLeft = nAlive - 1;

    while (chairsLeft >= 0) {
        checkSockets(chairsLeft);
    }
}

// ---------------------------------------------------
// checkSockets: poll all kid sockets once and
// handle any waiting WANT messages.
//   - ACK if the requested chair is free.
//   - NACK with available-chairs string if taken.
//   - QUIT if no chairs remain for this kid.
// chairsLeft is decremented on each ACK and set
// to -1 when a kid is eliminated.
// ---------------------------------------------------
void MomLogic::checkSockets(int& chairsLeft) {
    int status = poll(socks, nKids, -1);

    if (status < 0) {
        perror("poll");
        exit(1);
    }

    for (int k = 0; k < nKids; k++) {
        if (!players[k].alive) continue;
        if (socks[k].fd < 0) continue;
        if (!(socks[k].revents & POLLIN)) continue;

        char cmd[32];
        int seat;

        // Read the command word.
        if (fscanf(players[k].kidIn,
                   "%31s", cmd) != 1) {
            perror("reading WANT");
            exit(1);
        }

        if (strcmp(cmd, "WANT") != 0) {
            cerr << "Protocol error from "
                 << players[k].name << endl;
            exit(1);
        }

        // Read the requested seat number.
        if (fscanf(players[k].kidIn,
                   "%d", &seat) != 1) {
            perror("reading seat");
            exit(1);
        }

        printf("Mom <- %s: WANT %d\n",
               players[k].name.c_str(), seat);

        // If all chairs are already taken,
        // this kid is eliminated.
        if (chairsLeft == 0) {
            fprintf(players[k].kidOut, "QUIT\n");
            fflush(players[k].kidOut);

            printf("Mom -> %s: QUIT\n",
                   players[k].name.c_str());

            players[k].alive = false;

            // Close streams for this player,
            // then negate the fd for poll().
            closePlayer(k);
            socks[k].fd *= -1;

            nAlive--;
            chairsLeft = -1;
            return;
        }

        // Check whether the requested chair
        // is still available.
        if (seat >= 0 && chairs[seat] == '1') {
            chairs[seat] = '0';
            chairsLeft--;

            fprintf(players[k].kidOut, "ACK\n");
            fflush(players[k].kidOut);

            printf("Mom -> %s: ACK\n",
                   players[k].name.c_str());
        } else {
            // Chair taken; send NACK with the
            // current availability string.
            fprintf(players[k].kidOut,
                    "NACK %s\n", chairs);
            fflush(players[k].kidOut);

            printf("Mom -> %s: NACK %s\n",
                   players[k].name.c_str(), chairs);
        }
    }
}

// ---------------------------------------------------
// run: the main game loop.
// 1. Read HELLO from every kid.
// 2. Play rounds until one kid remains.
// 3. Send PRIZE to the winner.
// 4. Close all remaining sockets.
// ---------------------------------------------------
void MomLogic::run() {
    initPlayers();

    while (nAlive > 1) {
        initRound();
        stopTheMusic();
    }

    // Announce the winner.
    int winner = findWinner();

    if (winner >= 0) {
        fprintf(players[winner].kidOut, "PRIZE\n");
        fflush(players[winner].kidOut);

        printf("Mom -> %s: PRIZE\n",
               players[winner].name.c_str());
    }

    // Close all remaining sockets.
    // Negative fds belong to eliminated kids
    // whose streams were already closed.
    // closePlayer calls fclose on the streams,
    // which also closes the underlying fd.
    for (int k = 0; k < nKids; k++) {
        if (socks[k].fd < 0) continue;
        closePlayer(k);
    }
}
