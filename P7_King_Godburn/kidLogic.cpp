// kidLogic.cpp --------------------------------------
// Authors: Ryan Godburn and Emily King
// KidLogic class implementation for Musical Chairs
// with Sockets. Each action function validates
// the current state before processing the command.
// ---------------------------------------------------
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "kidLogic.hpp"

using namespace std;

// ---------------------------------------------------
// Constructor: store the socket fd and kid name.
// Streams are opened later in run().
// ---------------------------------------------------
KidLogic::KidLogic(int fd, const string& name) {
    momFd   = fd;
    kidName = name;
    state   = NEWBIE;
    nChairs = 0;
    momIn   = nullptr;
    momOut  = nullptr;
}

// ---------------------------------------------------
// Destructor: close FILE* streams if still open.
// fclose also closes the underlying fd, so we
// do NOT call close(momFd) separately.
// ---------------------------------------------------
KidLogic::~KidLogic() {
    if (momIn) {
        fclose(momIn);
        momIn = nullptr;
    }
    if (momOut) {
        fclose(momOut);
        momOut = nullptr;
    }
}

// ---------------------------------------------------
// run: open streams on the socket, send HELLO,
// then loop reading commands until state becomes
// QUITSTATE. Streams are closed by the destructor.
// ---------------------------------------------------
void KidLogic::run() {
    // Open two C-streams on the socket.
    // dup() gives the output stream its own fd
    // so fclose(momIn) won't close momOut's fd.
    momIn  = fdopen(momFd, "r");
    momOut = fdopen(dup(momFd), "w");

    if (!momIn || !momOut) {
        perror("fdopen");
        exit(1);
    }

    // First thing: identify ourselves to mom.
    sendHello();

    // Main command loop.
    while (state != QUITSTATE) {
        doCommand();
    }

    // Streams are closed by the destructor.
}

// ---------------------------------------------------
// sendHello: send the HELLO message with our name.
// ---------------------------------------------------
void KidLogic::sendHello() {
    fprintf(momOut, "HELLO %s\n", kidName.c_str());
    fflush(momOut);

    printf("%s -> Mom: HELLO %s\n",
           kidName.c_str(), kidName.c_str());
}

// ---------------------------------------------------
// doCommand: read one command from mom and
// dispatch it to the correct action function.
// Validates that the command is appropriate for
// the kid's current state. This gives a single
// point to catch protocol errors.
// ---------------------------------------------------
void KidLogic::doCommand() {
    int status = fscanf(momIn, "%31s", command);
    if (status != 1) {
        perror("reading command");
        exit(1);
    }

    cout << "State = " << stateName[state]
         << ",  Command is: " << command << endl;

    try {
        if      (strcmp("GETUP", command) == 0)
            doGetup();
        else if (strcmp("SIT",   command) == 0)
            doSit();
        else if (strcmp("NACK",  command) == 0)
            doNack();
        else if (strcmp("ACK",   command) == 0)
            doAck();
        else if (strcmp("QUIT",  command) == 0)
            doQuit();
        else if (strcmp("PRIZE", command) == 0)
            doPrize();
        else
            throw string("Protocol is mixed up.");
    }
    catch (string& s) {
        cout << s << " [" << command << "]\n";
        exit(1);
    }
}

// ---------------------------------------------------
// doGetup: receive the current number of chairs.
// Valid in NEWBIE (first round) or SITTING (after
// a successful previous round).
// ---------------------------------------------------
void KidLogic::doGetup() {
    if (state != NEWBIE && state != SITTING) {
        throw string("GETUP in wrong state");
    }

    if (fscanf(momIn, "%d", &nChairs) != 1) {
        perror("reading chairs");
        exit(1);
    }

    state = MARCHING;

    printf("%s: marching, chairs=%d\n",
           kidName.c_str(), nChairs);
}

// ---------------------------------------------------
// doSit: the music stopped. Pick a random chair
// and send a WANT request to mom. A random delay
// prevents the same kid from always winning.
// Valid only in MARCHING state.
// ---------------------------------------------------
void KidLogic::doSit() {
    if (state != MARCHING) {
        throw string("SIT in wrong state");
    }

    // Random delay for fairness.
    usleep((rand() % 30000) + 1);

    state = SEEKING;

    int choice = rand() % nChairs;
    sendWant(choice);
}

// ---------------------------------------------------
// sendWant: send a WANT message requesting the
// given chair number.
// ---------------------------------------------------
void KidLogic::sendWant(int chair) {
    fprintf(momOut, "WANT %d\n", chair);
    fflush(momOut);

    printf("%s -> Mom: WANT %d\n",
           kidName.c_str(), chair);
}

// ---------------------------------------------------
// doAck: mom approved our chair choice.
// Valid only in SEEKING state.
// ---------------------------------------------------
void KidLogic::doAck() {
    if (state != SEEKING) {
        throw string("ACK in wrong state");
    }

    state = SITTING;

    printf("%s: got a chair!\n", kidName.c_str());
}

// ---------------------------------------------------
// doNack: our chair was taken. Download the
// available-chairs string, find a free one using
// strchr, and send a new WANT.
// Valid only in SEEKING state.
// ---------------------------------------------------
void KidLogic::doNack() {
    if (state != SEEKING) {
        throw string("NACK in wrong state");
    }

    if (fscanf(momIn, "%255s", arg) != 1) {
        perror("reading NACK data");
        exit(1);
    }

    printf("%s: NACK received, available=%s\n",
           kidName.c_str(), arg);

    // Find the first available chair ('1').
    char* seatPtr = strchr(arg, '1');

    if (seatPtr == nullptr) {
        fprintf(stderr, "%s: no available chairs!\n",
                kidName.c_str());
        exit(1);
    }

    int newSeat = seatPtr - arg;

    printf("%s: retrying chair %d\n",
           kidName.c_str(), newSeat);

    sendWant(newSeat);
}

// ---------------------------------------------------
// doQuit: mom says we lost. Print a sad message
// and set state to QUITSTATE so run() terminates.
// Valid in SEEKING state (lost the round).
// ---------------------------------------------------
void KidLogic::doQuit() {
    if (state != SEEKING) {
        throw string("QUIT in wrong state");
    }

    printf("%s: I lost :(\n", kidName.c_str());

    state = QUITSTATE;
}

// ---------------------------------------------------
// doPrize: mom says we won the whole game!
// Valid in SITTING state (last kid alive).
// ---------------------------------------------------
void KidLogic::doPrize() {
    if (state != SITTING) {
        throw string("PRIZE in wrong state");
    }

    printf("%s: I WON THE GAME!\n",
           kidName.c_str());

    state = QUITSTATE;
}
