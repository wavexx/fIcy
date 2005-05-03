/*
 * match - match/filter a string according regex rules
 * Copyright(c) 2004-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef match_hh
#define match_hh

// system headers
#include <string>
#include <vector>
#include <regex.h>


/*
 * boolean class matcher
 *
 * incl and excl vectors are traversed using OR, and then combined with AND
 * (that is, both condition must be satisfacted with AT LEAST one regex).
 * for an empty incl set, the match is always accepted.
 */
class BMatch
{
  std::vector<regex_t> incl;
  std::vector<regex_t> excl;

  void
  compile(regex_t& data, const char* regex, int flags);

  void
  compInsert(std::vector<regex_t>& v, const char* regex, int flags);


public:
  // combine an inclusive operator
  void
  include(const char* regex, int flags = REG_EXTENDED)
  {
    compInsert(incl, regex, flags);
  }
  

  // combine an exclusive operator
  void
  exclude(const char* regex, int flags = REG_EXTENDED)
  {
    compInsert(excl, regex, flags);
  }


  bool
  empty() const
  {
    return (incl.empty() && excl.empty());
  }


  // load/append expressions from file
  void
  load(const char* file);

  // match a string against all regexes, according
  // to include/exclude expressions.
  bool
  operator()(const char* string, int flags = 0);
};

#endif
