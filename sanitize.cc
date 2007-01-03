/*
 * sanitize - rewrite a string to a suitable (secure) one - implementation
 * Copyright(c) 2003-2007 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "sanitize.hh"
using std::string;

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
#ifndef __APPLE__
    // non-ascii data is preserved (assumed to be locale-specific)
    if(!isascii(*it) || (isprint(*it) && *it != '/'))
      r += *it;
#else
    // OSX's HFS+ requires correct UTF-8m encoding, but we don't have the
    // source's encoding unless we parse Content-Encoding (from the HTTP
    // connection) and then recode according to LC_CTYPE (with Apple still
    // being an exception). We just strip for now. TODO: implement this.
    if(isprint(*it) && *it != '/')
      r += (*it & 0x7F);
#endif
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
