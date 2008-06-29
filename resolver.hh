/*
 * resolver - IN4/6 address resolver
 * Copyright(c) 2003-2008 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef resolver_hh
#define resolver_hh

// system headers
#include <cstddef>

// c system headers
#include <arpa/inet.h>


// resolve use gethostbyname internally, which is not reentrant
in_addr_t
resolve(const char* host, in_addr_t* addr = NULL);

#endif

