/*
 * rewrite - string rewriting functions through sed coproc. - implementation
 * Copyright(c) 2004 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 *
 * Note: we exploit here the -u (unbuffered) flag of GNU sed in order to avoid
 * fiddling with ptys and termio. This limitation should probably be removed.
 * The code looked so clean... *eeh*.
 */

// interface
#include "fIcy.hh"
#include "rewrite.hh"
using std::string;

#include <cstring>
using std::memcpy;

#include <stdexcept>
using std::runtime_error;

// c system headers
#include <unistd.h>
#include <sys/wait.h>


Rewrite::Rewrite(const char* arg, const arg_t type)
{
  // create two pipes
  if((pipe(in) < 0) || (pipe(out) < 0))
    throw runtime_error("cannot create pipes");

  pid = fork();
  if(pid < 0)
    throw runtime_error("cannot fork");

  if(pid)
  {
    // close unneeded ends
    close(in[1]);
    close(out[0]);
  }
  else
  {
    // close undeeded ends
    close(in[0]);
    close(out[1]);

    // remap descriptors
    if((dup2(in[1], STDOUT_FILENO) < 0) ||
	(dup2(out[0], STDIN_FILENO) < 0))
      throw runtime_error("cannot remap descriptors");

    // execute the coprocess
    execlp(fIcy::sed, fIcy::sed, "-u", (type == expr? "-e": "-f"), arg, NULL);
    throw runtime_error(string("cannot execute ") + fIcy::sed);
  }
}


Rewrite::~Rewrite() throw()
{
  // close and wait
  close(in[0]);
  close(out[1]);
  waitpid(pid, NULL, 0);
}


void
Rewrite::operator()(string& buf)
{
  // write to the coprocess
  char c = '\n';
  write(out[1], buf.data(), buf.size());
  if(write(out[1], &c, 1) < 0)
    throw runtime_error("cannot write to the coprocess");

  // read from the coprocess
  buf.clear();
  for(;; buf += c)
  {
    if(read(in[0], &c, 1) < 0)
      throw runtime_error("cannot read from the coprocess");
    if(c == '\n')
      break;
  }
}
