/*
 * urlParse - parse an url in proto/server/port/path components - impl.
 * Copyright(c) 2004-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 *
 * Supported URLs are in the form: [proto://]server[:port][/path]
 */

// interface
#include "urlparse.hh"
using std::string;

// c system headers
#include <stdlib.h>


// implementation
void
urlParse(string& proto, string& server, int& port, string& path,
    const string& url)
{
  // check for proto
  string::size_type protoEnd(url.find("://"));
  if(protoEnd == string::npos)
  {
    proto.clear();
    protoEnd = 0;
  }
  else
  {
    proto = url.substr(0, protoEnd);
    protoEnd += 3;
  }

  // search for path
  string::size_type slash(url.find('/', protoEnd));
  if(slash == string::npos)
    path = "/";
  else
    path = url.substr(slash);

  // port
  string::size_type colon(url.find(':', protoEnd));
  if(colon == string::npos || colon > slash)
  {
    colon = slash;
    port = 0;
  }
  else
    port = atoi(url.substr(colon + 1, slash).c_str());

  // finally, the server
  server = url.substr(protoEnd,
      (colon == string::npos? colon: colon - protoEnd));
}
