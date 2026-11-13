// main.cpp ------------------------------------------
// Authors: Ryan Godburn and Emily King
// Musical Chairs with Threads - Mom's code.
// Mom orchestrates the game: creates kids, runs
// rounds, eliminates losers, announces winner.
// ---------------------------------------------------
#include "kid.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <signal.h>

using namespace std;

// ---------------------------------------------------
// playOneRound: runs one complete round of the
// musical chairs game. Mom tells kids to march,
// stops the music, then finds the loser.
//
// Parameters:
//   mod        - pointer to the shared model
//   players    - array of Kid pointers
//   activeKids - how many kids still in game
// ---------------------------------------------------
void playOneRound(Model* mod,
    Kid* players[], int activeKids) {

    printf("\n");
    printf("==============================\n");
    printf("  Round: %d kids, %d chairs\n",
        activeKids, mod->nChairs);
    printf("==============================\n");

    // ---- Initialize chairs to empty ----
    pthread_mutex_lock(&mod->lock);
    for (int idx = 0;
         idx < mod->nChairs; idx++) {
        mod->chairs[idx] = -1;
    }
    mod->nMarching = 0;
    pthread_mutex_unlock(&mod->lock);

    // ---- Tell kids to stand up ----
    for (int idx = 0;
         idx < activeKids; idx++) {
        players[idx]->standUp();
    }

    // ---- Send SIGUSR1: start marching ----
    printf("Mom: telling kids to march\n");
    for (int idx = 0;
         idx < activeKids; idx++) {
        pthread_t kidTid =
            players[idx]->getTid();
        printf("Mom: SIGUSR1 -> Kid %d\n",
            players[idx]->getID());
        pthread_kill(kidTid, SIGUSR1);
    }

    // ---- Wait for all kids to march ----
    // Use condition variable to avoid busy
    // waiting. Signals are not queued, so
    // Mom checks the counter each time she
    // wakes up.
    pthread_mutex_lock(&mod->lock);
    while (mod->nMarching < activeKids) {
        pthread_cond_wait(
            &mod->marchCV, &mod->lock);
    }
    pthread_mutex_unlock(&mod->lock);
    printf("Mom: all %d kids marching\n",
        activeKids);

    // ---- Let them march a moment ----
    sleep(1);

    // ---- Stop the music ----
    printf("Mom: MUSIC STOPS!\n");

    // Reset counter before the sit phase
    pthread_mutex_lock(&mod->lock);
    mod->nMarching = 0;
    pthread_mutex_unlock(&mod->lock);

    // ---- Send SIGUSR2: grab a chair ----
    for (int idx = 0;
         idx < activeKids; idx++) {
        pthread_t kidTid =
            players[idx]->getTid();
        printf("Mom: SIGUSR2 -> Kid %d\n",
            players[idx]->getID());
        pthread_kill(kidTid, SIGUSR2);
    }

    // ---- Wait for all kids to respond ----
    pthread_mutex_lock(&mod->lock);
    while (mod->nMarching < activeKids) {
        pthread_cond_wait(
            &mod->sitCV, &mod->lock);
    }
    pthread_mutex_unlock(&mod->lock);
    printf("Mom: all kids responded\n");

    // ---- Find the standing kid (loser) ----
    // The loser has seatNumber == -1 and the
    // loser's thread has already exited.
    int loserIdx = -1;
    for (int idx = 0;
         idx < activeKids; idx++) {
        if (!players[idx]->isSeated()) {
            loserIdx = idx;
            break;
        }
    }

    if (loserIdx >= 0) {
        printf("Mom: Kid %d is OUT!\n",
            players[loserIdx]->getID());

        // Swap loser with last active player
        int lastIdx = activeKids - 1;
        if (loserIdx != lastIdx) {
            Kid* temp = players[loserIdx];
            players[loserIdx] =
                players[lastIdx];
            players[lastIdx] = temp;
        }

        // Remove one chair for next round
        mod->nChairs--;
        printf("Mom: %d chairs remain\n",
            mod->nChairs);
    }
}

// ---------------------------------------------------
// main: Mom sets up and runs the entire game.
// Reads nKids from command line, creates the
// model and kids, plays rounds, then cleans up.
// ---------------------------------------------------
int main(int argc, char* argv[]) {
    // ---- Validate command line ----
    if (argc != 2) {
        printf("Usage: %s numKids\n",
            argv[0]);
        return 1;
    }

    int numKids = atoi(argv[1]);
    if (numKids < 2) {
        printf("Error: need at least 2 kids\n");
        return 1;
    }

    // Save for joining later, since numKids
    // will be used to track active players.
    int totalKids = numKids;

    printf("=================================\n");
    printf("  Musical Chairs: %d players\n",
        numKids);
    printf("=================================\n");

    // Seed the random number generator
    srand(time(NULL));

    // ---- Block signals in main thread ----
    // Kid threads inherit this mask and use
    // sigwait() to receive signals cleanly.
    sigset_t blockSet;
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGUSR1);
    sigaddset(&blockSet, SIGUSR2);
    sigaddset(&blockSet, SIGQUIT);
    pthread_sigmask(
        SIG_BLOCK, &blockSet, NULL);

    // ---- Create the shared model ----
    // No locking needed yet because no
    // threads exist at this point.
    Model model(numKids);

    // ---- Create kid threads ----
    printf("Mom: creating %d kids\n",
        numKids);
    Kid** players = new Kid*[numKids];
    for (int idx = 0;
         idx < numKids; idx++) {
        players[idx] =
            new Kid(&model, idx);
    }

    // Brief pause so threads can start up
    // and reach their first sigwait() call.
    usleep(200000);

    // ---- Play rounds until one remains ----
    int activeKids = numKids;
    while (activeKids > 1) {
        playOneRound(
            &model, players, activeKids);
        activeKids--;
    }

    // ---- Announce the winner ----
    printf("\n");
    printf("=================================\n");
    printf("  WINNER: Kid %d gets the prize!\n",
        players[0]->getID());
    printf("=================================\n");

    // Tell the winner to go home
    printf("Mom: sending SIGQUIT to winner\n");
    pthread_kill(
        players[0]->getTid(), SIGQUIT);
    usleep(100000);

    // ---- Join all threads ----
    // Eliminated threads already exited, so
    // joining them returns immediately. The
    // winner exits after receiving SIGQUIT.
    printf("\nMom: joining all threads...\n");
    for (int idx = 0;
         idx < totalKids; idx++) {
        pthread_join(
            players[idx]->getTid(), NULL);
        printf("Mom: joined Kid %d\n",
            players[idx]->getID());
    }

    // ---- Clean up ----
    for (int idx = 0;
         idx < totalKids; idx++) {
        delete players[idx];
    }
    delete[] players;

    printf("\nMom: game over, goodbye!\n");

    // Use pthread_exit so any remaining
    // threads can finish cleanly.
    pthread_exit(NULL);
}
