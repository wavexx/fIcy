/*
 * sanitize - rewrite a string to a suitable (secure) one - implementation
 * Copyright(c) 2003-2004 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "sanitize.hh"
using std::string;

// system headers
#include <algorithm>
using std::remove_copy_if;

// c system headers
#include <ctype.h>


// implementation
string
sanitize_file(const string& src)
{
  string r;

  // discard initial dots
  string::const_iterator it(src.begin());
  while(it != src.end() && *it == '.')
    ++it;

  while(it != src.end())
  {
    // non-ascii data is preserved (assumed to be locale-specific)
    if(!isascii(*it) || (isprint(*it) && *it != '/'))
      r += *it;
    else
      r += '_';

    ++it;
  }

  return r;
}


string
sanitize_esc(const string& src)
{
  string r;

  for(string::const_iterator it = src.begin(); it != src.end(); ++it)
    if(!iscntrl(*it))
      r += *it;

  return r;
}
