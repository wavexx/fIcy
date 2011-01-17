/*
 * icy - protocol functions and constants - implementation
 * Copyright(c) 2003-2006 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
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
#include <string.h>


// implementation
namespace ICY
{
  namespace Proto
  {
    // global definitions
    const char* reqMeta = "icy-metadata: 1";
    const char* notice1 = "icy-notice1";
    const char* notice2 = "icy-notice2";
    const char* title = "icy-name";
    const char* genre = "icy-genre";
    const char* url = "icy-url";
    const char* br = "icy-br";
    const char* metaint = "icy-metaint";
    const char* mTitle = "StreamTitle";

    // local constants
    const char vaStart[] = "='";
    const size_t vaStartSz = sizeof(vaStart) / sizeof(*vaStart) - 1;
    const char vaEnd[] = "';";
    const size_t vaEndSz = sizeof(vaEnd) / sizeof(*vaEnd) - 1;
  }


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


  size_t
  Reader::dup(std::ostream* out, const size_t size, bool dup)
  {
    size_t r(0);

    while(r != size)
    {
      size_t p(in.read(buf, (bufSz + r > size)? size - r: bufSz));
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


  size_t
  Reader::readMeta(map<string, string>& meta)
  {
    // read the first byte containing the lenght
    char b;
    in.readn(&b, sizeof(b));

    size_t lenght(b * Proto::metaMul);
    if(lenght > Proto::metaSz)
      throw std::runtime_error("metadata stream lenght invalid");

    // metadata could be empty
    if(lenght)
    {
      in.readn(mBuf, lenght);

      // check for NULL termination
      if(mBuf[lenght - 1])
	throw std::runtime_error("invalid metadata stream");

      char* p(mBuf);
      char* n;

      /*
       * Now we parse the data. They're in the form name='value';name....
       * Unfortunatly, looking at the ICEcast and AMPLE code I do not
       * see any escape sequence for values containing "';" so I merely
       * look for terminators. Needless to say this is UGLY.
       */
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
