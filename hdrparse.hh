/*
 * hdrParse - parse HTTP headers
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef hdrparse_hh
#define hdrparse_hh

// local headers
#include "http.hh"

// system headers
#include <map>


namespace Http
{
  std::map<std::string, std::string>
  hdrParse(const Header& hdr);
}

#endif

