/*
 * icy - protocol functions and constants
 * Copyright(c) 2003-2006 of wave++ (Yuri D'Elia)
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
    const extern char* reqMeta;

    // some parameters we parse
    const extern char* notice1;
    const extern char* notice2;
    const extern char* title;
    const extern char* genre;
    const extern char* url;
    const extern char* br;
    const extern char* metaint;

    // metadata constants
    const size_t metaSz = 1024;
    const size_t metaMul = 16;

    // song title metatada
    const extern char* mTitle;
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
    size_t
    dup(std::ostream* out, const size_t size, bool dup = true);


    // metadata decoding
    size_t
    readMeta(std::map<std::string, std::string>& meta);
  };
}

#endif
