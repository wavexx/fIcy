/*
 * resolver - IN4/6 address resolver - implementation
 * Copyright(c) 2003-2008 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "resolver.hh"

// system headers
#include <stdexcept>

// c system headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


// implementation
in_addr_t
resolve(const char* host, in_addr_t* addr)
{
  // call the native gethostbyname
  hostent* he(gethostbyname(host));
  if(!he)
    throw std::runtime_error(hstrerror(h_errno));

  in_addr_t r;
  memcpy(&r, *he->h_addr_list, sizeof(r));
  if(addr)
    *addr = r;

  return r;
}

