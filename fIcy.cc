/*
 * fIcy - HTTP/1.0-ICY stream extractor/separator - implementation
 * Copyright(c) 2003-2005 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "fIcy.hh"
#include "icy.hh"
#include "htfollow.hh"
#include "sanitize.hh"
#include "tmparse.hh"
#include "rewrite.hh"
#include "match.hh"
#include "msg.hh"
using std::string;
using std::map;

// system headers
#include <iostream>
using std::cout;
using std::cerr;

#include <fstream>
using std::ofstream;

#include <stdexcept>
using std::runtime_error;

#include <limits>
#include <memory>
using std::auto_ptr;

// c system headers
#include <cstdlib>
#include <sys/types.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


// globals (urgh)
namespace
{
  bool dupStdout(true);
  char* lastFName(NULL);
  bool rmPartial(false);
}


void
shwIcyHdr(const map<string, string>& headers, const char* search,
    const char* name)
{
  map<string, string>::const_iterator it;
  if((it = headers.find(search)) != headers.end())
    cerr << name << it->second << std::endl;
}


// this helper function returns a new file opened for writing.
ofstream*
newFWrap(const char* file, const bool clobber)
{
  if(!clobber && !access(file, F_OK))
    return NULL;

  ofstream* out(new ofstream(file, std::ios_base::out));
  if(!*out)
  {
    delete out;
    throw runtime_error(string("cannot write `") + file + "'");
  }

  return out;
}


// a wrapper to the wrapper: if a file exists retry with a new file with
// an incremental number appended. changes file to the real name of the file
ofstream*
newNFWrap(string& file)
{
  ofstream* out(newFWrap(file.c_str(), false));
  if(!out)
  {
    char buf[16];
    for(size_t n = 0; n != std::numeric_limits<size_t>::max(); ++n)
    {
      snprintf(buf, sizeof(buf), ".%lu", n);
      if((out = newFWrap((file + buf).c_str(), false)))
      {
        file += buf;
        break;
      }
    }

    if(!out)
      throw runtime_error(string("no free files for `") + file + "'");
  }

  return out;
}


// SIGPIPE handler
void
sigPipe(const int)
{
  msg("broken pipe: disabling duping.");
  dupStdout = false;

  // close the descriptor: this is needed to release the resource
  close(STDOUT_FILENO);
}


// termination handler
void
sigTerm(const int sig)
{
  if(rmPartial && lastFName)
  {
    msg("removing incomplete last file: %s", lastFName);
    unlink(lastFName);
  }

  exit((sig == SIGTERM? Exit::success:
	  (sig == SIGPIPE? Exit::args:
	      Exit::fail)));
}


// signal installers
void
sigPipeInst()
{
  signal(SIGPIPE, SIG_IGN);
}


void
sigTermInst(const bool handlePipe)
{
  signal(SIGTERM, sigTerm);
  signal(SIGINT, sigTerm);
  signal(SIGHUP, sigTerm);
  if(handlePipe)
    signal(SIGPIPE, sigTerm);
}


// current song status
std::ostream&
hour(std::ostream& fd)
{
  fd.setf(std::ios_base::right);
  fd.width(2);
  fd.fill('0');

  return fd;
}


/*
 * function naming scheme: yep. I'm again in a new coding style internal fight.
 * But this time, I've been seriously tainted by functional/logical programming
 * so be patient. When I'll come up with clear ideas, I will uniform the code.
 */
time_t
display_status(const string& title, const size_t num, const time_t last)
{
  time_t now(time(NULL));

  if(last)
  {
    // show the song duration
    cerr << " [" << hour << ((now - last) / 60) << ":" <<
      hour << ((now - last) % 60) << "]\n";
  }

  cerr << "playing #" << num << ": " << title << std::flush;
  return now;
}


// update the sequence file
void
write_seq(ofstream& out, const char* file)
{
  if(out.rdbuf()->is_open())
  {
    out << file << std::endl;
    if(!out)
      throw runtime_error("cannot append to sequence file");
  }
}


// implementation
int
main(int argc, char* const argv[]) try
{
  // option defaults
  prg = argv[0];

  char* outFile = NULL;
  char* suffix = NULL;
  ofstream seq;
  bool enumFiles = false;
  bool nameFiles = false;
  bool showMeta = false;
  bool clobber = true;
  bool numEFiles = false;
  bool instSignal = false;
  time_t maxTime = 0;
  size_t maxFollow = fIcy::maxFollow;
  auto_ptr<Rewrite> rewrite;
  BMatch match;

  int arg;
  while((arg = getopt(argc, argv, "do:emvtcs:nprhq:x:X:I:f:F:M:l:")) != -1)
    switch(arg)
    {
    case 'd':
      dupStdout = false;
      break;

    case 'o':
      outFile = optarg;
      break;

    case 'e':
      enumFiles = true;
      break;

    case 'm':
      nameFiles = true;
      break;

    case 'v':
      verbose = true;
      break;

    case 't':
      showMeta = true;
      break;

    case 'c':
      clobber = false;
      break;

    case 's':
      suffix = optarg;
      break;

    case 'n':
      numEFiles = true;
      break;

    case 'p':
      instSignal = true;
      break;

    case 'r':
      rmPartial = true;
      break;

    case 'q':
      seq.open(optarg, std::ios_base::out | std::ios_base::app);
      if(!seq)
      {
	err("cannot open `%s' for appending", optarg);
	return Exit::fail;
      }
      break;

    case 'x':
      match.include(optarg);
      break;

    case 'X':
      match.exclude(optarg);
      break;

    case 'I':
      match.load(optarg);
      break;

    case 'f':
      rewrite.reset(new Rewrite(optarg, Rewrite::expr));
      break;

    case 'F':
      rewrite.reset(new Rewrite(optarg, Rewrite::file));
      break;

    case 'M':
      maxTime = tmParse(optarg);
      break;

    case 'l':
      maxFollow = atol(optarg);
      break;

    case 'h':
      cout << prg << fIcy::fIcyHelp << prg << " v" << fIcy::version <<
        " is\n" << fIcy::copyright;

    default:
      return Exit::args;
    }

  argc -= optind;
  if(argc < 1 || argc > 3)
  {
    err("bad parameters; see %s -h", prg);
    return Exit::args;
  }

  // connection parameters
  URL url;
  if(argc == 1)
  {
    // avoid the need of the protocol on the command line
    url = argv[optind++];
    if(!url.proto.size())
      url.proto = "http";
    else if(url.proto != "http")
      throw runtime_error(string("unknown protocol ") + url.proto);
  }
  else
  {
    // 1.0.4 compatibility
    url.proto = "http";
    url.server = argv[optind++];
    url.port = atoi(argv[optind++]);
    url.path = (argc == 3? argv[optind++]: "/");
  }

  // check for parameters consistency
  bool useMeta(enumFiles || nameFiles || !match.empty());
  bool reqMeta(useMeta || showMeta);

  // enumFiles and nameFiles requires a prefix
  if(useMeta && !outFile)
  {
    err("a prefix is required (see -o) when writing files");
    return Exit::args;
  }

  // you cannot disable duping if you don't write anything!
  if(!(outFile || dupStdout))
  {
    err("trying to perform a do-nothing download");
    return Exit::args;
  }

  // install the signals
  instSignal = (instSignal && dupStdout);
  rmPartial = (rmPartial && useMeta);
  sigTermInst(!(instSignal || rewrite.get()));
  if(instSignal)
    sigPipeInst();

  // setup headers
  Http::Header qHeaders;
  qHeaders.push_back(fIcy::userAgent);
  if(reqMeta)
    qHeaders.push_back(ICY::Proto::reqMeta);

  // establish the connection
  map<string, string> pReply;
  auto_ptr<Socket> s(htFollow(pReply, url, qHeaders, maxFollow));
  if(reqMeta && pReply.find(ICY::Proto::metaint) == pReply.end())
  {
    err("requested metadata, but got nothing.");
    return Exit::fail;
  }
  
  // show some headers
  if(verbose)
  {
    shwIcyHdr(pReply, ICY::Proto::notice1, "Notice: ");
    shwIcyHdr(pReply, ICY::Proto::notice2, "Notice: ");
    shwIcyHdr(pReply, ICY::Proto::title, "Title: ");
    shwIcyHdr(pReply, ICY::Proto::genre, "Genre: ");
    shwIcyHdr(pReply, ICY::Proto::url, "URL: ");
    shwIcyHdr(pReply, ICY::Proto::br, "Bit Rate: ");
  }

  // ensure the max playing time starts _after_ connect
  if(maxTime)
  {
    signal(SIGALRM, sigTerm);
    alarm(maxTime);
  }
  
  // start reading
  ssize_t metaInt(reqMeta?
      atol(pReply.find(ICY::Proto::metaint)->second.c_str()): fIcy::bufSz);
  ICY::Reader reader(*s, fIcy::bufSz);
  size_t enu(0);
  time_t tStamp(0);
  string lastTitle;
  
  // initial file
  auto_ptr<std::ostream> out;
  if(outFile && !useMeta)
    out.reset(newFWrap(outFile, clobber));
  else
    // the first filename is unknown as the metadata block will
    // arrive in the next metaInt bytes
    out.reset();

  for(;;)
  {
    if(dupStdout && !cout)
    {
      // try an empty write to determine if the file is ready
      if(!write(STDOUT_FILENO, NULL, 0))
	cout.clear();
    }
    
    // read the stream
    if(reader.dup(out.get(), metaInt, dupStdout) != metaInt)
    {
      msg("connection terminated");
      break;
    }
    if(!(outFile || dupStdout))
      break;
    
    // read metadata
    if(reqMeta)
    {
      map<string, string> data;
      if(reader.readMeta(data))
      {
	map<string, string>::const_iterator it(
	    data.find(ICY::Proto::mTitle));

	// de-uglify
	string title;
	if(it != data.end())
	{
	  // sanitize immediately, we don't want \n for sed
	  title = sanitize_esc(it->second);
	  if(rewrite.get())
	    (*rewrite)(title);
	}

	if(title.size() && (title != lastTitle))
	{
	  string newFName;
	  
	  if(showMeta)
	    tStamp = display_status(title, enu, tStamp);
	  
	  // skip the first filename generation when discarding partials
	  // or when the title doesn't match
	  if(useMeta && (enu || !rmPartial) && match(title.c_str()))
	  {
	    newFName = outFile;

	    if(enumFiles)
	    {
	      char buf[16];
	      snprintf(buf, sizeof(buf), (nameFiles? "[%lu] ": "%lu"), enu);
	      newFName += buf;
	    }
	    if(nameFiles)
	      newFName += sanitize_file(title);
	    if(suffix)
	      newFName += suffix;
	    
	    // open the new file
	    if(numEFiles)
	      out.reset(newNFWrap(newFName));
	    else
	      out.reset(newFWrap(newFName.c_str(), clobber));
	  }
	  else
	    out.reset();
	  
	  // update the last filename pointer
	  if(lastFName)
	  {
	    write_seq(seq, lastFName);
	    free(lastFName);
	  }
	  if(out.get())
	  {
	    lastFName = strdup(newFName.c_str());
	    msg("file changed to: %s", lastFName);
	  }
	  else if(lastFName)
	  {
	    lastFName = NULL;
	    msg("file reset");
	  }
	  
	  // update stream number
	  lastTitle = title;
	  ++enu;
	}
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
