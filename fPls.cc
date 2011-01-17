/*
 * fPls - playlist handler for fIcy
 * Copyright(c) 2004-2008 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "fIcy.hh"
#include "msg.hh"
#include "htfollow.hh"
#include "plsparse.hh"
#include "tmparse.hh"
#include "authparse.hh"
using std::string;
using std::list;
using std::map;

// system headers
#include <fstream>
using std::ifstream;

#include <sstream>
using std::istringstream;

#include <iostream>
using std::cout;

#include <memory>
using std::auto_ptr;

#include <stdexcept>
using std::runtime_error;

// c system headers
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>


/*
 * Parameters handling
 */
class Params
{
public:
  explicit
  Params(int argc, char* argv[]);

  ~Params() throw()
  {
    if(fIcyParams)
      delete[] fIcyParams;
  }


  const char* uri;
  const char** fIcyParams;
  size_t urlPos;
  size_t timePos;
  size_t maxRetries;
  long maxLoops;
  time_t waitSecs;
  time_t maxTime;
  time_t idleTime;
  size_t maxFollow;
  char* daemonize;
  char* auth;
  Http::Auth authData;
  bool help;
};


Params::Params(int argc, char* argv[])
{
  // defaults
  prg = argv[0];
  maxRetries = fIcy::maxRetries;
  maxLoops = fIcy::maxLoops;
  waitSecs = fIcy::waitSecs;
  maxTime = 0;
  maxFollow = fIcy::maxFollow;
  idleTime = 0;
  verbose = help = false;
  daemonize = NULL;
  auth = NULL;

  // locals
  const char* path = "fIcy";
  const char* maxFollowBuf = NULL;
  const char* idleTimeBuf = NULL;
  int arg;

  // let's again put a bit of SHAME... those FSC**BEEP GNU extensions.
  // WHY _NOT_ BEING POSIXLY_CORRECT BY DEFAULT EH? oooh, "features"! I see.
  while((arg = getopt(argc, argv, "+P:R:L:T:M:l:d:a:vhi:")) != -1)
    switch(arg)
    {
    case 'P':
      path = optarg;
      break;

    case 'R':
      maxRetries = strtoul(optarg, NULL, 0);
      break;

    case 'L':
      maxLoops = strtol(optarg, NULL, 0);
      break;

    case 'T':
      waitSecs = tmParse(optarg);
      break;

    case 'M':
      maxTime = tmParse(optarg);
      break;

    case 'l':
      maxFollowBuf = optarg;
      maxFollow = strtol(optarg, NULL, 0);
      break;

    case 'd':
      daemonize = optarg;
      break;

    case 'h':
      help = true;
      break;

    case 'a':
      auth = optarg;
      break;

    case 'v':
      verbose = true;
      break;

    case 'i':
      idleTimeBuf = optarg;
      idleTime = tmParse(optarg);
      break;
    }

  // fetch authentication tokens immediately to avoid further requests
  if(auth)
    authParse(authData, auth);

  // playlist
  argc -= optind;
  uri = argv[optind++];

  // fIcy parameters
  if(--argc >= 0)
  {
    fIcyParams = new const char*[argc + 13];
    fIcyParams[0] = path;
    arg = 1;

    // parameters we allow to override
    if(auth)
    {
      fIcyParams[arg++] = "-a";
      fIcyParams[arg++] = auth;
    }
    if(maxFollowBuf)
    {
      fIcyParams[arg++] = "-l";
      fIcyParams[arg++] = maxFollowBuf;
    }
    if(idleTimeBuf)
    {
      fIcyParams[arg++] = "-i";
      fIcyParams[arg++] = idleTimeBuf;
    }

    // forward other parameters
    for(int i = 1; i <= argc; ++i)
      fIcyParams[arg++] = argv[optind++];

    // imposed parameters
    fIcyParams[arg++] = "-M";
    timePos = arg++;

    if(daemonize)
      fIcyParams[arg++] = "-d";

    fIcyParams[arg++] = "--";
    urlPos = arg++;

    fIcyParams[arg++] = NULL;
  }
  else
    fIcyParams = NULL;
}


void
show_help()
{
  cout << prg << fIcy::fPlsHelp << prg << " v" << fIcy::version <<
    " is\n" << fIcy::copyright;
}


void
load_file(string& out, const char* file)
{
  ifstream in(file);
  if(!in)
    throw runtime_error(string("cannot open ") + file);

  // load the file
  char buf[fIcy::bufSz];

  do
  {
    in.read(buf, sizeof(buf));
    out.append(buf, in.gcount());
  }
  while(in);

  if(!in.eof())
    throw runtime_error(string("errors while reading from ") + file);
}


void
load_file(string& out, const URL& url, const size_t maxFollow,
    const time_t idleTime, const Http::Auth* auth)
{
  // setup headers
  Http::Header qHeaders;
  qHeaders.push_back(fIcy::userAgent);
  if(auth)
    qHeaders.push_back(auth->basicHeader());

  // connection
  map<string, string> pReply;
  auto_ptr<Socket> s(htFollow(pReply, url, qHeaders, maxFollow, idleTime));

  // load the file
  char buf[fIcy::bufSz];
  size_t ret;

  while((ret = s->read(buf, sizeof(buf))) > 0)
    out.append(buf, ret);
}


void
load_list(string& buf, const char* uri, const size_t maxFollow,
    const time_t idleTime, const Http::Auth* auth)
{
  URL url(uri);

  if(url.proto == Http::Proto::proto)
    load_file(buf, url, maxFollow, idleTime, auth);
  else if(!url.proto.size())
  {
    msg("loading playlist from %s", uri);
    load_file(buf, uri);
  }
  else
    throw runtime_error(string("unknown protocol ") + url.proto);
}


// exec fIcy and return the exit code
int
exec_fIcy(Params& params, const time_t maxTime, const char* stream)
{
  const char** args = params.fIcyParams;
  char buf[16];
  snprintf(buf, sizeof(buf), "%lu", maxTime);
  args[params.timePos] = buf;
  args[params.urlPos] = stream;
  int ret;

  switch(fork())
  {
  case -1:
    err("cannot fork");
    return Exit::args;

  case 0:
    execvp(args[0], const_cast<char**>(args));
    err("cannot exec fIcy: %s", strerror(errno));
    exit(Exit::args);
  }

  wait(&ret);
  return (WIFEXITED(ret)? WEXITSTATUS(ret): Exit::args);
}


void
daemonize(const char* logFile)
{
  int fd;

  // ensure correctness before daemonization
  if(logFile)
    if((fd = open(logFile, O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1)
      throw runtime_error((string("cannot open log file ") + logFile).c_str());

  // daemonize
#ifdef __sgi
  if(_daemonize(0, fd, -1, -1) == -1)
#else
  if(daemon(0, 0) == -1)
#endif
    throw runtime_error("cannot daemonize process");

  // fix stderr
  if(logFile)
    dup2(fd, STDERR_FILENO);
}


int
main(int argc, char* argv[]) try
{
  // initialize
  Params params(argc, argv);
  if(!params.uri && !params.help)
  {
    err("bad parameters; see %s -h", prg);
    return Exit::args;
  }
  if(params.help)
  {
    show_help();
    return Exit::args;
  }

  // fetch and parse the playlist
  list<string> playlist;
  {
    string buf;
    load_list(buf, params.uri, params.maxFollow, params.idleTime,
	(params.auth? &params.authData: NULL));
    istringstream stream(buf);
    plsParse(playlist, stream);
  }
  msg("loaded %d elements", playlist.size());

  // daemonize now if requested
  if(params.daemonize)
    daemonize(params.daemonize);

  // residual playing time
  time_t resTime(params.maxTime);

  // main loop
  for(size_t loop = 0; static_cast<long>(loop) != params.maxLoops; ++loop)
  {
    for(list<string>::const_iterator it = playlist.begin();
	it != playlist.end(); ++it)
    {
      for(size_t retry = 0; retry != params.maxRetries; ++retry)
      {
	// sweet dreams on temporary failures
	if(loop || it != playlist.begin())
	  sleep(params.waitSecs);

	time_t start(time(NULL));
	msg("stream %s: retry %d of loop %d", it->c_str(), retry + 1, loop + 1);

	int ret = exec_fIcy(params, resTime, it->c_str());
	if(ret == Exit::args)
	  return Exit::fail;

	// update residual
	if(params.maxTime)
	{
	  time_t d(time(NULL) - start + params.waitSecs);
	  if(d >= resTime)
	    return ret;
	  resTime -= d;
	}

	// check for success after maxTime
	if(ret == Exit::success)
	  break;
      }
    }
  }

  return Exit::success;
}
catch(runtime_error& err)
{
  ::err("%s", err.what());
  return Exit::fail;
}
catch(...)
{
  err("unknown error, aborting");
  abort();
}
