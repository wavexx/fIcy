/*
 * sanitize - rewrite a string to a suitable (secure) filename - implementation
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "sanitize.hh"
using std::string;

// c system headers
#include <ctype.h>


// implementation
string
sanitize(const string& src)
{
  string r;

  // discard initial dots
  string::const_iterator it(src.begin());
  while(it != src.end() && *it == '.')
    ++it;

  while(it != src.end())
  {
    if(isprint(*it) && *it != '/')
      r += *it;
    else
      r += '_';

    ++it;
  }

  return r;
}

