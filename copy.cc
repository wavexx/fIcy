/*
 * copy - stream 2 stream buffered copy - implementation
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "copy.hh"
using std::size_t;
using std::istream;
using std::ostream;

// system headers
#include <memory>
using std::auto_ptr;


// implementation
bool
copy(ostream& out, istream& in, const size_t& len, const size_t& bufSz)
{
  // check for bad buffer sizes
  if(!bufSz)
    return true;

  auto_ptr<char> buf(new char[bufSz]);
  size_t trSz(len);
  for(; bufSz <= trSz; trSz -= bufSz)
  {
    in.read(buf.get(), bufSz);
    if(!in)
      return true;

    out.write(buf.get(), bufSz);
    if(!out)
      return true;
  }
  
  if(trSz)
  {
    in.read(buf.get(), trSz);
    if(!in)
      return true;

    out.write(buf.get(), trSz);
    if(!out)
      return true;
  }

  return false;
}
