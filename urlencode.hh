/*
 * urlEncode - encode an 8bit-clean string to a 7bit clean url
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef urlencode_hh
#define urlencode_hh

// system headers
#include <string>


namespace Http
{
  std::string
  urlEncode(const std::string& src, const bool clean = false);
}

#endif

