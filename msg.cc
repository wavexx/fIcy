/*
 * msg - user messages - implementation
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "msg.hh"

// c system headers
#include <stdarg.h>
#include <stdio.h>


// data
const char* prg;
bool verbose = false;


// implementation
void
err(const char* fmt, ...)
{
  va_list params;

  fprintf(stderr, "%s: ", prg);

  va_start(params, fmt);
  vfprintf(stderr, fmt, params);
  va_end(params);

  fprintf(stderr, "\n");
}


void
msg(const char* fmt, ...)
{
  if(!verbose)
    return;

  va_list params;

  fprintf(stderr, "%s: ", prg);
  
  va_start(params, fmt);
  vfprintf(stderr, fmt, params);
  va_end(params);
  
  fprintf(stderr, "\n");
}
