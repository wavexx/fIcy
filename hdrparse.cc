/*
 * hdrParse - parse HTTP headers - implementation
 * Copyright(c) 2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "hdrparse.hh"
using std::string;
using std::map;


// implementation
namespace Http
{
  map<string, string>
  hdrParse(const Header& hdr)
  {
    map<string, string> m;

    for(Header::const_iterator it = hdr.begin(); it != hdr.end(); ++it)
    {
      string::size_type p(it->find(':'));
      if(p == string::npos || (p + 1) > it->size())
	continue;

      // non-compliant strings without whitespace are handled
      short spc = ((p + 2) < it->size() && (*it)[p + 1] == ' '? 2: 1);
      m.insert(std::make_pair(it->substr(0, p), it->substr(p + spc)));
    }

    return m;
  }
}

