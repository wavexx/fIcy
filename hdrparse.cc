/*
 * hdrParse - parse HTTP headers - implementation
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
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
      if(p != string::npos)
        m.insert(std::make_pair(it->substr(0, p), it->substr(p + 1)));
    }

    return m;
  }
}

