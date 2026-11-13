// model.h -------------------------------------------
// Authors: Ryan Godburn and Emily King
// Model struct for Musical Chairs.
// Shared data accessed by Mom and all Kid threads.
// ---------------------------------------------------
#ifndef MODEL_H
#define MODEL_H

#include <pthread.h>
#include <cstdlib>
#include <cstdio>

using namespace std;

// ---------------------------------------------------
// The Model holds all shared game state.
// It is a struct so all members are public.
// ---------------------------------------------------
struct Model {
    pthread_mutex_t lock;   // protects model
    pthread_cond_t marchCV; // kids signal march
    pthread_cond_t sitCV;   // kids signal sit
    int nMarching;          // response counter
    int nChairs;            // chairs this round
    int* chairs;            // chair array

    // ------------------------------------------
    // Constructor: initialize the model.
    // Parameter numKids is the kid count.
    // nChairs is always one less than numKids.
    // ------------------------------------------
    Model(int numKids) {
        nChairs = numKids - 1;
        nMarching = 0;
        chairs = new int[nChairs];
        for (int idx = 0; idx < nChairs; idx++)
            chairs[idx] = -1;
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&marchCV, NULL);
        pthread_cond_init(&sitCV, NULL);
        printf("Model: %d chairs allocated\n",
            nChairs);
    }

    // ------------------------------------------
    // Destructor: free the chair array and
    // destroy synchronization primitives.
    // ------------------------------------------
    ~Model() {
        delete[] chairs;
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&marchCV);
        pthread_cond_destroy(&sitCV);
        printf("Model: destroyed\n");
    }
};

#endif
