/*
 * plsParse - M3U/EXTM3U/PLS/PLSv2 playlist parser/s - implementation
 * Copyright(c) 2004-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "plsparse.hh"
using std::string;
using std::istream;
using std::list;

// system headers
#include <stdexcept>
using std::runtime_error;


// read a line ignoring ms-dos crud and spaces, whatever the platform
istream&
getRealLine(istream& fd, string& buf)
{
  istream& ret(getline(fd, buf));
  string::size_type pos;

  pos = buf.find_first_not_of(" \t");
  if(pos != string::npos && pos)
    buf = buf.substr(pos);

  pos = buf.find_last_not_of(" \r\t");
  if(pos != string::npos)
    buf.resize(pos + 1);

  return ret;
}

// like getRealLine, but strips INI comments too
istream&
getIniLine(istream& fd, string& buf)
{
  istream& ret(getline(fd, buf));
  string::size_type pos;

  pos = buf.find_first_not_of(" \t");
  if(pos != string::npos && pos)
    buf = buf.substr(pos);

  pos = buf.find(';');
  if(pos != string::npos)
    buf.resize(pos);

  pos = buf.find_last_not_of(" \r\t");
  if(pos != string::npos)
    buf.resize(pos + 1);

  return ret;
}

void
m3uParse(std::list<std::string>& list, std::istream& fd)
{
  string buf;
  while(getRealLine(fd, buf))
    list.push_back(buf);
}


void
extm3uParse(std::list<std::string>& list, std::istream& fd)
{
  string buf;

  // discard the first line
  getRealLine(fd, buf);
  if(buf != "#EXTM3U")
    throw runtime_error("not an extm3u file");

  while(getRealLine(fd, buf))
  {
    if(!buf.compare(0, 8, "#EXTINF:"))
      continue;

    list.push_back(buf);
  }
}

void
plsv2Parse(std::list<std::string>& list, std::istream& fd)
{
  string buf;

  // discard the first line
  getRealLine(fd, buf);
  if(buf != "[playlist]")
    throw runtime_error("not a plsv2 file");

  while(getIniLine(fd, buf))
  {
    // section changes (not permitted!)
    if(!buf.compare(0, 1, "["))
      break;

    // file ordering is irrilevant for us
    if(!buf.compare(0, 4, "File"))
    {
      string::size_type eq(buf.find('=', 4));
      if(eq != string::npos)
	list.push_back(buf.substr(eq + 1));
    }
  }
}


void
plsParse(std::list<std::string>& list, std::istream& fd)
{
  // get the first line
  string buf;
  getRealLine(fd, buf);
  fd.seekg(0);

  // identify the stream
  if(buf == "[playlist]")
    plsv2Parse(list, fd);
  else if(buf == "#EXTM3U")
    extm3uParse(list, fd);
  else
    m3uParse(list, fd);
}
