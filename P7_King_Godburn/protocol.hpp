// protocol.hpp --------------------------------------
// Authors: Ryan Godburn and Emily King
// Shared protocol definitions for Program 7.
// Both mom (server) and kid (client) include this
// file. It defines the port number, limits, and
// the protocol message types.
// ---------------------------------------------------
#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

// ---------------------------------------------------
// Port where mom listens for kid connections.
// ---------------------------------------------------
const int PORTNUM = 1234;

// ---------------------------------------------------
// Maximum number of kids allowed in the game.
// ---------------------------------------------------
const int MAX_KIDS = 25;

// ---------------------------------------------------
// Maximum length of a protocol message buffer.
// ---------------------------------------------------
const int MAX_MSG = 256;

// ---------------------------------------------------
// Commands used in the socket protocol.
//   HELLO  - kid -> mom on first connect
//   GETUP  - mom -> kid at start of round
//   SIT    - mom -> kid when music stops
//   WANT   - kid -> mom requesting a chair
//   ACK    - mom -> kid approving chair choice
//   NACK   - mom -> kid rejecting chair choice
//   QUIT   - mom -> kid when eliminated
//   PRIZE  - mom -> last kid standing
// ---------------------------------------------------
enum CommandT {
    CMD_HELLO,
    CMD_GETUP,
    CMD_SIT,
    CMD_WANT,
    CMD_ACK,
    CMD_NACK,
    CMD_QUIT,
    CMD_PRIZE,
    CMD_UNKNOWN
};

// ---------------------------------------------------
// Kid states during the game.
//   NEWBIE   - just connected, waiting for GETUP
//   MARCHING - received GETUP, marching around
//   SEEKING  - received SIT, trying to find a chair
//   SITTING  - received ACK, seated safely
//   QUITSTATE- received QUIT or PRIZE, done
// ---------------------------------------------------
enum StateT {
    NEWBIE,
    MARCHING,
    SEEKING,
    SITTING,
    QUITSTATE
};

// ---------------------------------------------------
// Human-readable names for kid states.
// Used in debug output.
// ---------------------------------------------------
const char* const stateName[] = {
    "NEWBIE",
    "MARCHING",
    "SEEKING",
    "SITTING",
    "QUIT"
};

#endif
