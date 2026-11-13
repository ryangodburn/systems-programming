// kid.h ---------------------------------------------
// Authors: Ryan Godburn and Emily King
// Kid class declaration for Musical Chairs.
// Each Kid runs as its own thread.
// ---------------------------------------------------
#ifndef KID_H
#define KID_H

#include "model.h"
#include <signal.h>
#include <pthread.h>

// Forward declaration of Kid class.
class Kid;

// ---------------------------------------------------
// Global function: entry point for kid threads.
// Required because pthread_create needs a global
// or static function pointer.
// ---------------------------------------------------
void* startThread(void* kidPtr);

// ---------------------------------------------------
// Global function: translate a signal number
// into a human-readable string.
// ---------------------------------------------------
const char* sigName(int sig);

// ---------------------------------------------------
// The Kid class encapsulates all data and behavior
// for one child playing musical chairs.
// ---------------------------------------------------
class Kid {
private:
    Model* model;        // ptr to shared model
    int idNumber;        // this kid's ID
    pthread_t tid;       // this kid's thread ID
    sigset_t signalSet;  // signals to listen for
    int wantSeat;        // desired chair index
    int seatNumber;      // chair obtained (-1=none)

public:
    // Constructor: creates the kid's thread.
    Kid(Model* mod, int idNum);

    // Predicate: true if kid has a chair.
    bool isSeated() const;

    // Mutator: Mom calls to reset kid's seat.
    void standUp();

    // Handle SIGUSR1: begin marching.
    void doMarch();

    // Handle SIGUSR2: try to find a chair.
    void doSit();

    // Main thread function: signal loop.
    void play();

    // Getters for ID and thread ID.
    int getID() const;
    pthread_t getTid() const;
};

#endif
