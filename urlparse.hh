/*
 * urlParse - parse an url in proto/server/port/path components
 * Copyright(c) 2004-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef urlparse_hh
#define urlparse_hh

// system headers
#include <string>


// underlying parsers
void
urlParse(std::string& proto, std::string& server, int& port, std::string& path,
    const std::string& url);


// common structures
struct URL
{
  // constructors
  URL()
  {}

  URL(const std::string& proto, const std::string& server,
      const int port, const std::string& path)
  : proto(proto), server(server), port(port), path(path)
  {}

  URL(const std::string& url)
  {
    urlParse(proto, server, port, path, url);
  }


  // helpers
  URL&
  operator =(const std::string& url)
  {
    urlParse(proto, server, port, path, url);
    return *this;
  }


  // data
  std::string proto;
  std::string server;
  int port;
  std::string path;
};

#endif
