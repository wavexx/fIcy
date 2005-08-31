/*
 * sockets - streams sockets
 * Copyright(c) 2003-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef socket_hh
#define socket_hh

// c system headers
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

// not all systems define sock_t
#ifndef sock_t
typedef int sock_t;
#endif


class Socket
{
  bool conn;

protected:
  sock_t fd;

public:
  Socket(const in_addr_t& host, const int port)
  : conn(false)
  {
    open(host, port);
  }

  Socket(const char* host, const int port)
  : conn(false)
  {
    open(host, port);
  }

  Socket()
  : conn(false)
  {}

  ~Socket();

  void
  open(const char* host, const int port);
  
  void
  open(const in_addr_t& host, const int port);

  void
  close(const int how = 0);

  ssize_t
  read(char* buffer, const size_t lenght, const timeval* timeout = NULL);

  ssize_t
  gets(char* buffer, const size_t lenght,
      const char term = '\n', const timeval* timeout = NULL);

  ssize_t
  write(const char* buffer, const size_t lenght);
};

#endif

