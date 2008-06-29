/*
 * rewrite - string rewriting functions through coproc.
 * Copyright(c) 2004-2008 of wave++ (Yuri D'Elia)
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
  int fd;

public:
  enum arg_t {expr, file};

  explicit
  Rewrite(const char* arg, const char* coproc, const arg_t type);

  ~Rewrite() throw();

  void
  operator()(std::string& buf);
};

#endif
