/*
 * HTTP/1.0 for streams sockets
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef http_hh
#define http_hh

// local headers
#include "socket.hh"

// system headers
#include <vector>
#include <string>


// protocol constants
namespace Http
{
  namespace Proto
  {
    const char proto[] = "http";
    const char protoTy[] = "tcp";
    const char version[] = "HTTP/1.0";
    const size_t hdrLen = 1024;
    const char get[] = "GET";

    const char endl[] = "\r\n";
    const size_t endlSz = sizeof(endl) / sizeof(*endl) - 1;
  }


  // some useful typedefs
  typedef std::vector<std::string> Header;


  // interface
  class Http
  {
    char* host;
    in_addr_t addr;
    int port;

    // http functions
    int
    getSrvPort();


  protected:
    // generic operations
    Socket*
    gen(const char* act, const char* path, const Header* headers = NULL,
        Header* reply = NULL);


  public:
    // de/constructors
    Http(const char* host, const int port = 0);
    ~Http();

    // basic functionality
    Socket*
    get(const char* file, const Header* headers = NULL, Header* reply = NULL);
  };
}

#endif

