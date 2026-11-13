// kidLogic.hpp --------------------------------------
// Authors: Ryan Godburn and Emily King
// KidLogic class declaration for Musical Chairs
// with Sockets. Manages the kid's protocol state
// and communication with mom.
// ---------------------------------------------------
#ifndef KIDLOGIC_HPP
#define KIDLOGIC_HPP

#include <string>
#include <cstdio>
#include "protocol.hpp"

// ---------------------------------------------------
// KidLogic: encapsulates all state and behavior
// for one child (client) in the game.
// ---------------------------------------------------
class KidLogic {
private:
    int momFd;            // socket file descriptor
    FILE* momIn;          // input stream from mom
    FILE* momOut;         // output stream to mom

    int nChairs;          // chairs in current round
    std::string kidName;  // this kid's name
    StateT state;         // current protocol state

    char command[32];     // buffer for command word
    char arg[256];        // buffer for extra data

public:
    // Constructor: store fd and name.
    KidLogic(int fd, const std::string& name);

    // Destructor: close streams.
    ~KidLogic();

    // Main loop: open streams, send HELLO,
    // process commands until done.
    void run();

    // Send the initial HELLO message.
    void sendHello();

    // Read one command and dispatch it.
    void doCommand();

    // Protocol action functions.
    void doGetup();   // handle GETUP
    void doSit();     // handle SIT
    void doAck();     // handle ACK
    void doNack();    // handle NACK
    void doQuit();    // handle QUIT
    void doPrize();   // handle PRIZE

    // Helper: send a WANT message.
    void sendWant(int chair);
};

#endif
