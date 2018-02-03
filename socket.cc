/*
 * sockets - streams sockets - implementation
 * Copyright(c) 2003-2018 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "socket.hh"

// system headers
#include <stdexcept>

// c system headers
#include <unistd.h>
#include <string.h>
#include <errno.h>


// implementation
Socket::~Socket() throw()
{
  if(conn)
  {
    close();
    if(timeout)
      delete timeout;
  }
}


void
Socket::open(const addrinfo& ai, const timeval* timeout)
{
  if(conn)
    close();

  fd = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
  if(fd == -1)
    throw std::runtime_error(strerror(errno));

  if(::connect(fd, ai.ai_addr, ai.ai_addrlen) == -1)
    throw std::runtime_error(strerror(errno));

  if(!timeout)
    this->timeout = NULL;
  else
  {
    this->timeout = new timeval;
    *this->timeout = *timeout;
  }

  conn = true;
}


void
Socket::open(const char* host, const char* port, const timeval* timeout)
{
  // resolve the hostname
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  addrinfo* ai;
  int r = getaddrinfo(host, port, &hints, &ai);
  if(r)
    throw std::runtime_error(gai_strerror(r));

  // connect
  try { open(*ai, timeout); }
  catch(...)
  {
    freeaddrinfo(ai);
    throw;
  }

  freeaddrinfo(ai);
}


void
Socket::close(const int how)
{
  // do _not_ check return status of close: as close is called under
  // the destructor it could be fatal.
  if(how)
  {
    ::shutdown(fd, how);
    if(how == SHUT_RDWR)
      conn = false;
  }
  else
  {
    ::close(fd);
    conn = false;
  }
}


size_t
Socket::read(char* buffer, const size_t lenght)
{
  if(timeout)
  {
    // reinitialize for threading
    timeval t = *timeout;

    fd_set fdSet;
    FD_ZERO(&fdSet);
    FD_SET(fd, &fdSet);

    int rt = select(fd + 1, &fdSet, NULL, NULL, &t);
    if(rt < 0)
      throw std::runtime_error(strerror(errno));
    if(!rt)
      throw std::runtime_error("select timeout");
  }

  ssize_t b = ::recv(fd, buffer, lenght, 0);
  if(b == -1)
    throw std::runtime_error(strerror(errno));

  return b;
}


void
Socket::readn(char* buffer, const size_t lenght)
{
  size_t res = lenght;

  while(res)
  {
    size_t n = read(buffer, res);
    if(!n)
      throw std::runtime_error("connection terminated prematurely");

    res -= n;
    buffer += n;
  }
}


size_t
Socket::gets(char* buffer, const size_t lenght, const char term)
{
  // gets works under a non-buffered stream, so it's particularly slow
  char* w = buffer;
  size_t pos = 0;
  char b;

  while(pos != lenght)
  {
    if(read(&b, 1) != 1)
      break;

    *w++ = b;
    ++pos;

    if(b == term)
      break;
  }

  return pos;
}


size_t
Socket::write(const char* buffer, const size_t lenght)
{
  if(timeout)
  {
    // reinitialize for threading
    timeval t = *timeout;

    fd_set fdSet;
    FD_ZERO(&fdSet);
    FD_SET(fd, &fdSet);

    int rt = select(fd + 1, NULL, &fdSet, NULL, &t);
    if(rt < 0)
      throw std::runtime_error(strerror(errno));
    if(!rt)
      throw std::runtime_error("select timeout");
  }

  ssize_t b = ::send(fd, buffer, lenght, 0);
  if(b == -1)
    throw std::runtime_error(strerror(errno));

  return b;
}


void
Socket::writen(const char* buffer, const size_t lenght)
{
  size_t res = lenght;

  while(res)
  {
    size_t n = write(buffer, res);
    if(!n)
      throw std::runtime_error("connection terminated prematurely");

    res -= n;
    buffer += n;
  }
}
