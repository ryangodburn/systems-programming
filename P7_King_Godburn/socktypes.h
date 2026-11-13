// socktypes.h ---------------------------------------
// Authors: Ryan Godburn and Emily King
// Typedefs for system socket types and comments
// describing the socket data structures used in
// the Musical Chairs socket program.
// ---------------------------------------------------
#ifndef SOCKTYPES_H
#define SOCKTYPES_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>

// ---------------------------------------------------
// sockaddr_in: Internet socket address structure.
//   sin_family  - address family (AF_INET)
//   sin_port    - port number in network byte order
//   sin_addr    - IP address (struct in_addr)
// ---------------------------------------------------

// ---------------------------------------------------
// pollfd: structure for the poll() system call.
//   fd      - file descriptor to monitor
//   events  - requested events (POLLIN, POLLOUT)
//   revents - returned events after poll()
// When a connection is eliminated, we negate the fd
// so that poll() will ignore it:
//   socks[k].fd *= -1;
// ---------------------------------------------------

// ---------------------------------------------------
// hostent: returned by gethostbyname().
//   h_name      - official host name
//   h_addr_list - list of addresses
//   h_length    - length of each address
// ---------------------------------------------------

// Convenient typedef for socket address casting.
typedef struct sockaddr_in SockAddrIn;
typedef struct sockaddr    SockAddr;

#endif
