/*
 * urlEncode - encode an 8bit-clean string to a 7bit clean url - implementation
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "urlencode.hh"
using std::string;

// c system headers
#include <stdio.h>


// implementation
namespace Http
{
  string
  urlEncode(const string& src, const bool clean)
  {
    string rt;
  
    for(string::const_iterator it = src.begin(); it != src.end(); ++it)
    {
      if(clean || (*it > 32 && *it < 123))
        rt += *it;
      else
      {
        char buf[5];
        snprintf(buf, sizeof(buf), "%%%X", *it);
        rt += buf;
      }
    }
  
    return rt;
  }
}

