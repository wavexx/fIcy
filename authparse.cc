/*
 * authparse - parse/get HTTP credentials - implementation
 * Copyright(c) 2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "authparse.hh"
using std::string;

// system headers
#include <fstream>
using std::ifstream;
using std::getline;

#include <stdexcept>
using std::runtime_error;


// implementation
void
authParse(Http::Auth& out, const char* file)
{
  string buf;
  ifstream in(file);
  if(!in || !getline(in, buf))
    throw runtime_error(string("cannot read credentials from ") + file);

  string::size_type userEnd(buf.find(':'));
  if(userEnd == string::npos)
    throw runtime_error("user:password separator ':' not found");

  out.user = buf.substr(0, userEnd);
  if(!out.user.size())
    throw runtime_error("bad user token");

  out.pass = buf.substr(userEnd + 1);
}
