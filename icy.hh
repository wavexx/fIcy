/*
 * icy - protocol functions and constants
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef icy_hh
#define icy_hh

// local headers
#include "socket.hh"

// system headers
#include <iostream>
#include <map>
#include <string>


namespace ICY
{
  namespace Proto
  {
    // particular HTTP header to be sent when requiring metadata
    const char reqMeta[] = "icy-metadata: 1";

    // some parameters we parse
    const char notice1[] = "icy-notice1";
    const char notice2[] = "icy-notice2";
    const char title[] = "icy-name";
    const char genre[] = "icy-genre";
    const char url[] = "icy-url";
    const char br[] = "icy-br";
    const char metaint[] = "icy-metaint";

    // metadata constants
    const size_t metaSz(1024);
    const size_t metaMul(16);

    // song title metatada
    const char mTitle[] = "StreamTitle";

    // variable parameters
    const char vaStart[] = "='";
    const size_t vaStartSz = sizeof(vaStart) / sizeof(*vaStart) - 1;
    const char vaEnd[] = "';";
    const size_t vaEndSz = sizeof(vaEnd) / sizeof(*vaEnd) - 1;
  }

  
  // interface
  class Reader
  {
    Socket& in;
    char* buf;
    char* mBuf;
    size_t bufSz;
    size_t mBufSz;


  public:
    Reader(Socket& in, const size_t bufSz,
        const size_t metaSz = Proto::metaSz);
    ~Reader();


    // duping function
    ssize_t
    dup(std::ostream* out, const size_t size, bool dup = true);


    // metadata decoding
    ssize_t
    readMeta(std::map<std::string, std::string>& meta);
  };
}

#endif
