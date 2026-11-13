// momLogic.hpp --------------------------------------
// Authors: Ryan Godburn and Emily King
// MomLogic class declaration for Musical Chairs
// with Sockets. Contains the business logic for
// the server side of the game, separated from
// the connection setup in mom.cpp.
// ---------------------------------------------------
#ifndef MOMLOGIC_HPP
#define MOMLOGIC_HPP

#include <string>
#include <cstdio>
#include <poll.h>

// ---------------------------------------------------
// Player: stores per-kid connection info and state.
// The two FILE* streams enable high-level I/O
// (fprintf / fscanf) on top of the raw socket.
// ---------------------------------------------------
struct Player {
    std::string name = "??";
    bool alive = false;
    FILE* kidIn  = nullptr;  // stream: kid -> mom
    FILE* kidOut = nullptr;  // stream: mom -> kid
};

// ---------------------------------------------------
// MomLogic: orchestrates the musical chairs game.
// Manages the chair array, the player list, and
// runs rounds until one kid remains.
// ---------------------------------------------------
class MomLogic {
private:
    int nKids;        // total kids at game start
    int nAlive;       // kids still in the game
    char* chairs;     // chair string ('1'=free '0'=taken)
    pollfd* socks;    // polling array, one per kid
    Player* players;  // player info array

public:
    // Constructor: allocate chairs and players.
    MomLogic(int nk, pollfd sockArray[]);

    // Destructor: free dynamic memory and close
    // any open FILE* streams.
    ~MomLogic();

    // Main game loop: read HELLOs, play rounds,
    // send PRIZE, close sockets.
    void run();

    // Read HELLO messages and open streams.
    void initPlayers();

    // Set up chairs and send GETUP to all kids.
    void initRound();

    // Send SIT and process WANT requests via poll.
    void stopTheMusic();

    // Check each socket once for WANT messages.
    void checkSockets(int& chairsLeft);

    // Close streams for a specific player.
    void closePlayer(int k);

    // Count how many kids are still alive.
    int countAlive() const;

    // Find the index of the surviving kid.
    int findWinner() const;
};

#endif
