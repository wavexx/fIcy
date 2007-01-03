/*
 * rewrite - string rewriting functions through sed coproc. - implementation
 * Copyright(c) 2004-2006 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
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
#include <sys/param.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>

// pseudo-master device
#if (!defined(_POSIX_VERSION) || (_POSIX_VERSION < 200112L))
#include <paths.h>
#define PATH_PTMX _PATH_DEV "ptmx"
#endif

// streams
#ifndef BSD
#include <sys/ioctl.h>
#include <stropts.h>
#endif


// partial replacement to satisfy our needs (forkpty isn't supported enough)
pid_t
forkpty(int* amaster, char*, termios*, winsize*)
{
  pid_t pid;
  int masterFd;

  // open the master device and fork
#ifndef PATH_PTMX
  if((masterFd = posix_openpt(O_RDWR)) < 0)
    return -1;
#else
  if((masterFd = open(PATH_PTMX, O_RDWR)) < 0)
    return -1;
#endif

  if(grantpt(masterFd) < 0 || unlockpt(masterFd) < 0 || (pid = fork()) < 0)
  {
    close(masterFd);
    return -1;
  }

  if(pid)
    *amaster = masterFd;
  else
  {
    // establish as a session leader
    setsid();

    const char* slavePath;
    int slaveFd;

    if(!(slavePath = ptsname(masterFd)) ||
	(slaveFd = open(slavePath, O_RDWR)) < 0)
    {
      close(masterFd);
      return -1;
    }

    close(masterFd);

#ifndef BSD
    if(ioctl(slaveFd, I_PUSH, "ptem") < 0 ||
	ioctl(slaveFd, I_PUSH, "ldterm") < 0)
    {
      close(slaveFd);
      return -1;
    }

    // not all systems have ttcompat
    ioctl(slaveFd, I_PUSH, "ttcompat");
#endif

    dup2(slaveFd, STDIN_FILENO);
    dup2(slaveFd, STDOUT_FILENO);
    dup2(slaveFd, STDERR_FILENO);
    close(slaveFd);
  }

  return pid;
}


Rewrite::Rewrite(const char* arg, const arg_t type)
{
  pid = forkpty(&fd, NULL, NULL, NULL);
  if(pid < 0)
    throw runtime_error(string("cannot initialize slave pty: ")
	+ strerror(errno));

  if(!pid)
  {
    // setup the terminal environment
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    cfmakeraw(&t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW | TCSAFLUSH, &t);

    // execute the coprocess
    execlp(fIcy::sed, fIcy::sed, (type == expr? "-e": "-f"), arg, NULL);
    throw runtime_error(string("cannot execute ") + fIcy::sed);
  }
}


Rewrite::~Rewrite() throw()
{
  // close and wait
  close(fd);
  waitpid(pid, NULL, 0);
}


void
Rewrite::operator()(string& buf)
{
  // write to the coprocess
  write(fd, buf.data(), buf.size());
  if(write(fd, "\n", 1) < 0)
    throw runtime_error("cannot write to the coprocess");

  // read from the coprocess
  buf.clear();
  char c;
  for(;; buf += c)
  {
    if(read(fd, &c, 1) < 0)
      throw runtime_error("cannot read from the coprocess");
    if(c == '\n')
      break;
  }
}
