/*
 * icy - protocol functions and constants - implementation
 * Copyright(c) 2003-2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "icy.hh"
using std::ostream;
using std::map;
using std::string;
using std::cout;

// system headers
#include <stdexcept>

// c system headers
#include <cstring>


// implementation
namespace ICY
{
  Reader::Reader(Socket& in, const size_t bufSz, const size_t metaSz)
  : in(in), bufSz(bufSz), mBufSz(metaSz)
  {
    buf = new char[bufSz];
    mBuf = new char[mBufSz];
  }


  Reader::~Reader()
  {
    delete []buf;
    delete []mBuf;
  }


  ssize_t
  Reader::dup(std::ostream* out, const size_t size, bool dup)
  {
    size_t r(0);
    
    while(r != size)
    {
      ssize_t p(in.read(buf, (bufSz + r > size)? size - r: bufSz));
      if(!p)
        break;

      if(out)
        out->write(buf, p);
      if(dup)
      {
	// check for writing errors, but do not throw exceptions: SIGPIPE
	// should be generated already.
        cout.write(buf, p);
	if(!cout)
	  dup = false;
	else
	  cout.flush();
      }

      r += p;
    }

    return r;
  }


  ssize_t
  Reader::readMeta(map<string, string>& meta)
  {
    // read the first byte containing the lenght
    char b;
    if(!in.read(&b, sizeof(b)))
      throw std::runtime_error("connection terminated prematurely");

    size_t lenght(b * Proto::metaMul);
    if(lenght > Proto::metaSz)
      throw std::runtime_error("metadata stream lenght invalid");

    // metadata could be empty
    if(lenght)
    {
      size_t r(0);
      while(r != lenght)
      {
        ssize_t b(in.read(mBuf + r, lenght - r));
        if(!b)
          throw std::runtime_error("connection terminated prematurely");

        r += b;
      }

      // check for NULL termination
      if(mBuf[lenght - 1])
        throw std::runtime_error("invalid metadata stream");

      char* p(mBuf);
      char* n;

      while(*p)
      {
        // variable name
        n = strstr(p, Proto::vaStart);
        if(!n)
          break;
        string name(p, n);
        p = n + Proto::vaStartSz;

        // value
        n = strstr(p, Proto::vaEnd);
        if(!n)
          break;
        string value(p, n);
        p = n + Proto::vaEndSz;

        meta.insert(std::make_pair(name, value));
      }
    }

    return lenght;
  }
}

