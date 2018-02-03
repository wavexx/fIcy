/*
 * sockets - streams sockets
 * Copyright(c) 2003-2018 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef socket_hh
#define socket_hh

// system headers
#include <cstddef>

// c system headers
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>

// not all systems define sock_t
#if !defined(_SOCK_T) && !defined(sock_t)
#define _SOCK_T
typedef int sock_t;
#endif


class Socket
{
  bool conn;
  timeval* timeout;

protected:
  sock_t fd;

public:
  Socket(const addrinfo& ai, const timeval* timeout = NULL)
  : conn(false)
  {
    open(ai, timeout);
  }

  Socket(const char* host, const char* port, const timeval* timeout = NULL)
  : conn(false)
  {
    open(host, port, timeout);
  }

  Socket()
  : conn(false)
  {}

  ~Socket() throw();

  void
  open(const char* host, const char* port, const timeval* timeout = NULL);

  void
  open(const addrinfo& ai, const timeval* timeout = NULL);

  void
  close(const int how = 0);

  size_t
  read(char* buffer, const size_t lenght);

  void
  readn(char* buffer, const size_t lenght);

  size_t
  gets(char* buffer, const size_t lenght, const char term = '\n');

  size_t
  write(const char* buffer, const size_t lenght);

  void
  writen(const char* buffer, const size_t lenght);
};

#endif
