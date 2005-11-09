/*
 * match - match/filter a string according regex rules - implementation
 * Copyright(c) 2004-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "match.hh"
using std::string;
using std::vector;

// system headers
#include <fstream>
using std::ifstream;
using std::getline;

#include <stdexcept>
using std::runtime_error;

// c system headers
#include <stdlib.h>


// compile a regular expression with error checking
void
BMatch::compile(regex_t& data, const char* regex, int flags)
{
  int res;

  if((res = regcomp(&data, regex, flags | REG_NOSUB)))
  {
    char buf[128];
    regerror(res, &data, buf, sizeof(buf));
    throw runtime_error(string("regcomp: ") + buf);
  }
}


void
BMatch::compInsert(std::vector<regex_t>& v, const char* regex, int flags)
{
  regex_t buf;
  compile(buf, regex, flags);
  v.push_back(buf);
}


void
BMatch::load(const char* file)
{
  ifstream in(file);
  if(!in)
    throw runtime_error(string("can't open: ") + file);

  string line;
  char buf[16];

  for(size_t num = 1; getline(in, line); ++num)
  {
    if(!line.size() || line[0] == '#')
      continue;

    if(line[0] == '+')
      include(line.substr(1).c_str());
    else if(line[0] == '-')
      exclude(line.substr(1).c_str());
    else
    {
      snprintf(buf, sizeof(buf), ":%lu", num);
      throw runtime_error(string("syntax error in ") + file + buf);
    }
  }
}


bool
BMatch::operator()(const char* string, int flags)
{
  bool retIncl(!incl.size());
  for(vector<regex_t>::const_iterator it = incl.begin(); it != incl.end(); ++it)
    if(!regexec(&*it, string, 0, NULL, flags))
    {
      retIncl = true;
      break;
    }
  if(!retIncl)
    return false;

  bool retExcl(true);
  for(vector<regex_t>::const_iterator it = excl.begin(); it != excl.end(); ++it)
    if(!regexec(&*it, string, 0, NULL, flags))
    {
      retExcl = false;
      break;
    }
  
  return retExcl;
}
