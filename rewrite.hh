/*
 * rewrite - string rewriting functions through sed coproc.
 * Copyright(c) 2004 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef rewrite_hh
#define rewrite_hh

// system headers
#include <string>

// c system headers
#include <sys/types.h>


class Rewrite
{
  pid_t pid;
  int in[2];
  int out[2];

public:
  explicit
  Rewrite(const char* script = NULL);

  ~Rewrite() throw();

  void
  operator()(std::string& buf);
};

#endif
