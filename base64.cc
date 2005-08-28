/*
 * base64 - a simple base64 encoder - implementation
 * Copyright(c) 2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "base64.hh"
using std::string;


// conversion tables
namespace
{
  const char b64[64] =
  {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
  };
}


std::string
base64Encode(const std::string& src)
{
  string rt;

  // a very simple encoder with carry
  char carry = 0;
  char bits = 0;

  for(string::const_iterator it = src.begin(); it != src.end();)
  {
    const unsigned char& s(*it);

    bits += 2;
    rt += b64[carry | (s >> bits)];

    if(bits == 8)
    {
      carry = 0;
      bits = 0;
    }
    else
    {
      carry = (s << (6 - bits)) & 0x3f;
      ++it;
    }
  }

  // padding
  if(bits)
  {
    rt += b64[static_cast<int>(carry)];
    while((bits += 2) != 8)
      rt += '=';
  }

  return rt;
}
