/*
 * fPls - playlist handler for fIcy
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "fIcy.hh"
#include "msg.hh"
#include "http.hh"
#include "urlparse.hh"
#include "plsparse.hh"
using std::string;
using std::list;

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
#include <cstdlib>
using std::strtoul;
using std::strtol;

#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


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
  char** fIcyParams;
  size_t paramPos;
  size_t maxRetries;
  long maxLoops;
  size_t waitSecs;
  bool help;
};


Params::Params(int argc, char* argv[])
{
  // defaults
  prg = argv[0];
  char* path = "fIcy";
  maxRetries = fIcy::maxRetries;
  maxLoops = fIcy::maxLoops;
  waitSecs = fIcy::waitSecs;
  verbose = help = false;

  // real values
  int arg;

  // let's again put a bit of SHAME... that FSC**BEEP GNU extensions.
  // WHY _NOT_ BEING POSIXLY_CORRECT BY DEFAULT EH? oooh, "features"! I see.
  while((arg = getopt(argc, argv, "+P:R:L:T:vh")) != -1)
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
      waitSecs = strtoul(optarg, NULL, 0);
      break;

    case 'h':
      help = true;
      break;

    case 'v':
      verbose = true;
      break;
    }

  // playlist
  argc -= optind;
  uri = argv[optind++];

  // fIcy parameters
  if(--argc >= 0)
  {
    fIcyParams = new char*[argc + 4];
    fIcyParams[0] = path;
    fIcyParams[argc + 1] = "--";
    paramPos = argc + 2;
    fIcyParams[argc + 3] = NULL;

    for(int i = 1; i <= argc; ++i)
      fIcyParams[i] = argv[optind++];
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
load_file(string& out, const string server, const int port, const string path)
{
  Http::Http httpc(server.c_str(), port);
    
  // setup headers
  Http::Header qHeaders;
  Http::Reply reply;
  
  // query
  qHeaders.push_back(fIcy::userAgent);
  auto_ptr<Socket> s(httpc.get(path.c_str(), reply, &qHeaders));

  // load the file
  char buf[fIcy::bufSz];
  ssize_t ret;
 
  while((ret = s->read(buf, sizeof(buf))) > 0)
    out.append(buf, ret);
}


void
load_list(string& buf, const char* uri)
{
  string proto;
  string server;
  int port;
  string path;

  urlParse(proto, server, port, path, uri);
  if(proto.size() && proto == "http")
  {
    msg("loading playlist from (%s %d)", server.c_str(), port);
    load_file(buf, server, port, path);
  }
  else if(!proto.size())
  {
    msg("loading playlist from %s", uri);
    load_file(buf, uri);
  }
}


// exec fIcy and return the exit code
int
exec_fIcy(Params& params, const char* stream)
{
  char** args = params.fIcyParams;
  args[params.paramPos] = const_cast<char*>(stream);
  int ret;

  switch(fork())
  {
  case -1:
    err("cannot fork");
    return Exit::args;

  case 0:
    execvp(args[0], args);
    err("cannot exec fIcy: %s", strerror(errno));
    exit(Exit::args);
  }

  wait(&ret);
  return WEXITSTATUS(ret);
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
    load_list(buf, params.uri);
    istringstream stream(buf);
    plsParse(playlist, stream);
  }
  msg("loaded %d elements", playlist.size());

  // main loop
  for(size_t loop = 0; static_cast<long>(loop) != params.maxLoops; ++loop)
  {
    for(list<string>::const_iterator it = playlist.begin();
	it != playlist.end(); ++it)
    {
      for(size_t retry = 0; retry != params.maxRetries; ++retry)
      {
	msg("stream %s, retry %d of loop %d", it->c_str(), retry + 1, loop + 1);
	int ret = exec_fIcy(params, it->c_str());
	if(ret == Exit::args)
	  return Exit::fail;
	else if(ret == Exit::success)
	  break;

	// temporary failure, wait a bit
	sleep(params.waitSecs);
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
