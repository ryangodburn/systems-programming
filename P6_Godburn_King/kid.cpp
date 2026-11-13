// kid.cpp -------------------------------------------
// Authors: Ryan Godburn and Emily King
// Kid class implementation for Musical Chairs.
// Contains the Kid member functions and the two
// required global functions.
// ---------------------------------------------------
#include "kid.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

using namespace std;

// ---------------------------------------------------
// Global function: entry point for each kid thread.
// Casts the generic void* back to Kid* and calls
// the kid's play() function.
// ---------------------------------------------------
void* startThread(void* kidPtr) {
    Kid* kp = (Kid*) kidPtr;
    kp->play();
    return nullptr;
}

// ---------------------------------------------------
// Global function: returns a string name for
// the given signal number.
// ---------------------------------------------------
const char* sigName(int sig) {
    switch (sig) {
        case SIGUSR1:
            return "SIGUSR1";
        case SIGUSR2:
            return "SIGUSR2";
        case SIGQUIT:
            return "SIGQUIT";
        default:
            return "Unexpected signal";
    }
}

// ---------------------------------------------------
// Kid constructor.
// Initializes fields, sets up the signal mask,
// and creates a thread for this kid.
// ---------------------------------------------------
Kid::Kid(Model* mod, int idNum)
    : model(mod), idNumber(idNum),
      wantSeat(-1), seatNumber(-1) {

    // Build signal set: listen for these three
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGUSR1);
    sigaddset(&signalSet, SIGUSR2);
    sigaddset(&signalSet, SIGQUIT);

    // Create this kid's thread
    pthread_create(
        &tid, NULL, startThread, (void*)this
    );
    printf("  Kid %d: thread created, "
        "tid=%lu\n",
        idNumber, (unsigned long)tid);
}

// ---------------------------------------------------
// Predicate: returns true if this kid is seated
// (has a valid chair number).
// ---------------------------------------------------
bool Kid::isSeated() const {
    return seatNumber >= 0;
}

// ---------------------------------------------------
// standUp: Mom calls this at the start of each
// round to reset the kid to standing.
// ---------------------------------------------------
void Kid::standUp() {
    seatNumber = -1;
}

// ---------------------------------------------------
// doMarch: called when kid gets SIGUSR1.
// Pick a random target chair, then tell Mom
// that this kid is now marching.
// ---------------------------------------------------
void Kid::doMarch() {
    // Choose a random chair to aim for
    wantSeat = rand() % model->nChairs;
    printf("  Kid %d: marching, "
        "wants chair %d\n",
        idNumber, wantSeat);

    // Lock, increment counter, signal Mom
    pthread_mutex_lock(&model->lock);
    model->nMarching++;
    pthread_cond_signal(&model->marchCV);
    pthread_mutex_unlock(&model->lock);
}

// ---------------------------------------------------
// doSit: called when kid gets SIGUSR2.
// The music has stopped. Try to find and claim
// a free chair. If none found, this kid is
// eliminated and the thread exits.
// ---------------------------------------------------
void Kid::doSit() {
    // Random delay for fairness so the last
    // kid in line has a reasonable chance.
    int delayMs = rand() % 50;
    usleep(delayMs * 1000);

    // Lock the model for the entire search.
    // This prevents another kid from grabbing
    // the same chair between our check and
    // our claim.
    pthread_mutex_lock(&model->lock);

    int numChairs = model->nChairs;
    int found = -1;

    // Search circularly starting at wantSeat
    for (int count = 0;
         count < numChairs; count++) {
        int chairIdx =
            (wantSeat + count) % numChairs;
        if (model->chairs[chairIdx] == -1) {
            // Chair is empty; claim it
            model->chairs[chairIdx] = idNumber;
            found = chairIdx;
            break;
        }
    }

    if (found >= 0) {
        // Success: record which chair we got
        seatNumber = found;
        printf("  Kid %d: sat in chair %d\n",
            idNumber, seatNumber);
    } else {
        // Failure: no chair available
        seatNumber = -1;
        printf("  Kid %d: no chair, "
            "eliminated!\n", idNumber);
    }

    // Either way, tell Mom we are done
    model->nMarching++;
    pthread_cond_signal(&model->sitCV);
    pthread_mutex_unlock(&model->lock);

    // If eliminated, exit this thread now
    if (found < 0) {
        printf("  Kid %d: thread exiting\n",
            idNumber);
        pthread_exit(NULL);
    }
}

// ---------------------------------------------------
// play: the main function for each kid's thread.
// Loops forever, waiting for signals from Mom.
// SIGUSR1 = start marching
// SIGUSR2 = music stopped, grab a chair
// SIGQUIT = game over, go home
// ---------------------------------------------------
void Kid::play() {
    int receivedSig;

    printf("  Kid %d: ready to play\n",
        idNumber);

    for (;;) {
        // Block until Mom sends a signal
        sigwait(&signalSet, &receivedSig);
        printf("  Kid %d: received %s\n",
            idNumber,
            sigName(receivedSig));

        if (receivedSig == SIGUSR1) {
            doMarch();
        }
        else if (receivedSig == SIGUSR2) {
            doSit();
        }
        else if (receivedSig == SIGQUIT) {
            printf("  Kid %d: going home\n",
                idNumber);
            pthread_exit(NULL);
        }
    }
}

// ---------------------------------------------------
// Getter: return this kid's ID number.
// ---------------------------------------------------
int Kid::getID() const {
    return idNumber;
}

// ---------------------------------------------------
// Getter: return this kid's thread ID.
// ---------------------------------------------------
pthread_t Kid::getTid() const {
    return tid;
}
