/*
 * tmParse - parse a time quantity
 * Copyright(c) 2005-2006 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 *
 * Tries to parse time quantities in the form HH:MM[:SS] or SS[dhms]
 */

// interface
#include "tmparse.hh"

// system headers
#include <stdexcept>
using std::runtime_error;

// c system headers
#include <stdio.h>
#include <ctype.h>


// implementation
time_t
tmParse(const char* spec)
{
  // try the HH:MM[:SS] format first
  unsigned long h, m, s;
  int args;

  if((args = sscanf(spec, "%lu:%lu:%lu", &h, &m, &s)) > 1)
    return (h * 3600 + m * 60 + (args == 3? s: 0));

  // didn't match, try SS[dhms]
  char c;
  double n;

  if((args = sscanf(spec, "%lf %c", &n, &c)) > 0)
  {
    if(args == 2)
    {
      switch(tolower(c))
      {
      case 's': break;
      case 'm': n *= 60.; break;
      case 'h': n *= 3600.; break;
      case 'd': n *= 86400.; break;
      default: throw runtime_error("unknown time multiplier");
      }
    }

    return static_cast<time_t>(n);
  }

  // what else?
  throw runtime_error("time spec not understood");
}
