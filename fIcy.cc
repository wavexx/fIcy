/*
 * fIcy - HTTP/1.0-ICY stream extractor/separator - implementation
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "fIcy.hh"
#include "icy.hh"
#include "http.hh"
#include "hdrparse.hh"
#include "sanitize.hh"
using std::string;
using std::map;

// system headers
#include <iostream>
using std::cout;
using std::cerr;

#include <fstream>
using std::ofstream;

#include <stdexcept>
#include <limits>
#include <memory>
using std::auto_ptr;

// c system headers
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>


// constants (urgh)
const char* prg;
bool verbose(false);


// some functions
void
err(const char* fmt, ...)
{
  va_list params;

  fprintf(stderr, "%s: ", prg);

  va_start(params, fmt);
  vfprintf(stderr, fmt, params);
  va_end(params);

  fprintf(stderr, "\n");
}


void
msg(const char* fmt, ...)
{
  if(verbose)
  {
    va_list params;

    fprintf(stderr, "%s: ", prg);

    va_start(params, fmt);
    vfprintf(stderr, fmt, params);
    va_end(params);

    fprintf(stderr, "\n");
  }
}


void
shwIcyHdr(const map<string, string>& headers, const char* search,
    const char* name)
{
  map<string, string>::const_iterator it;
  if((it = headers.find(search)) != headers.end())
    cerr << name << it->second << std::endl;
}


// this helper function returns a new file opened for writing. errors during
// open or creat are (eventually) thrown as runtime_errors.
ofstream*
newFWrap(const char* file, const bool clobber = true, const bool ign = false)
{
  if(!access(file, F_OK) && !clobber)
  {
    if(ign)
      return NULL;
    else
      throw std::runtime_error(string("cannot clobber existing file: ") + file);
  }

  ofstream* out(new ofstream(file, std::ios_base::out));
  if(!*out)
  {
    delete out;

    if(ign)
      out = NULL;
    else
      throw std::runtime_error(string("cannot write `") + file + "'");
  }

  return out;
}


// a wrapper to the wrapper: if a file exists retry with a new file with
// an incremental number appended. changes file to the real name of the file
ofstream*
newNFWrap(string& file, const bool ign = false)
{
  ofstream* out(newFWrap(file.c_str(), false, true));
  if(!out)
  {
    char buf[16];
    for(size_t n = 0; n != std::numeric_limits<size_t>::max(); ++n)
    {
      snprintf(buf, sizeof(buf), ".%lu", n);
      if(out = newFWrap((file + buf).c_str(), false, true))
      {
        file += buf;
        break;
      }
    }

    if(!(out || ign))
      throw std::runtime_error(string("no free files for `") + file + "'");
  }

  return out;
}


// implementation
int
main(int argc, char* const argv[])
{
  // option defaults
  prg = argv[0];

  bool dupStdout(true);
  char* outFile(NULL);
  char* suffix(NULL);
  bool enuFiles(false);
  bool useMeta(false);
  bool showMeta(false);
  bool clobber(true);
  bool ignFErr(false);
  bool numEFiles(false);

  int arg;
  while((arg = getopt(argc, argv, "do:emvtcs:inh")) != -1)
    switch(arg)
    {
    case 'd':
      dupStdout = false;
      break;

    case 'o':
      outFile = optarg;
      break;

    case 'e':
      enuFiles = true;
      break;

    case 'm':
      useMeta = true;
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

    case 'i':
      ignFErr = true;
      break;

    case 'n':
      numEFiles = true;
      break;

    case 'h':
      cout << prg << fIcy::help << prg << " v" << fIcy::version <<
        " is\n" << fIcy::copyright;
      return Exit::success;

    default:
      return Exit::args;
    }

  argc -= optind;
  if(argc < 1 || argc > 3)
  {
    err("bad parameters; see %s -h", prg);
    return Exit::args;
  }

  const char* server(argv[optind++]);
  const int port(atoi(argv[optind++]));
  const char* path((argc == 3)? argv[optind++]: "/");
  bool reqMeta(enuFiles || useMeta || showMeta);

  // check for parameters consistency
  // enuFiles and useMeta requires a prefix
  if((useMeta || enuFiles) && !outFile)
  {
    err("a prefix is required (see -o) when writing multiple files");
    return Exit::args;
  }

  // you cannot disable duping if you don't write anything!
  if(!(outFile || dupStdout))
  {
    err("trying to perform a do-nothing download");
    return Exit::args;
  }

  try
  {
    // resolve the hostname
    msg("connecting to (%s %d)", server, port);
    Http::Http httpc(server, port);
    
    // setup headers
    Http::Header headers;
    Http::Header reply;

    headers.push_back(fIcy::userAgent);
    if(reqMeta)
      headers.push_back(ICY::Proto::reqMeta);

    msg("requesting stream on (%s)", path);
    auto_ptr<Socket> s(httpc.get(path, &headers, &reply));

    // validate the reply
    map<string, string> pReply(Http::hdrParse(reply));
    if(reqMeta && (pReply.find(ICY::Proto::metaint) == pReply.end()) &&
        reply[0].compare(0, ICY::Proto::okSz, ICY::Proto::ok))
    {
        err("unexpected ICY reply");
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
    
    // start reading
    size_t metaInt(reqMeta?
        atol(pReply.find(ICY::Proto::metaint)->second.c_str()): fIcy::bufSz);
    ICY::Reader reader(*s, fIcy::bufSz);
    size_t enu(0);

    // initial file
    auto_ptr<std::ostream> out;
    if(outFile && !(enuFiles || useMeta))
      out.reset(newFWrap(outFile, clobber));
    else
      // the first filename is unknown as the metadata block will
      // arrive in the next metaInt bytes
      out.reset();

    for(;;)
    {
      // read the stream
      if(reader.dup(out.get(), metaInt, dupStdout) != metaInt)
      {
        msg("connection terminated");
        break;
      }

      // read metadata
      if(reqMeta)
      {
        map<std::string, std::string> data;
        if(reader.readMeta(data))
        {
          map<std::string, std::string>::const_iterator title(
              data.find(ICY::Proto::mTitle));
          if(title != data.end() && title->second.size() > 0)
          {
            if(showMeta)
              cerr << "current song: " << title->second << std::endl;

            if(enuFiles || useMeta)
            {
              string newFName(outFile);
              if(enuFiles)
              {
                char buf[16];
                snprintf(buf, sizeof(buf), "[%lu] ", enu);
                newFName += buf;
                ++enu;
              }
              if(useMeta)
                newFName += sanitize(title->second);
              if(suffix)
                newFName += suffix;

              // open the new file
              if(numEFiles)
                out.reset(newNFWrap(newFName, ignFErr));
              else
                out.reset(newFWrap(newFName.c_str(), clobber, ignFErr));

              if(out.get())
                msg("file changed to: %s", newFName.c_str());
            }
          }
        }
      }
    }
  }
  catch(std::runtime_error& err)
  {
    ::err("software caused error: %s", err.what());
    return Exit::fail;
  }
  catch(...)
  {
    err("unknown error, aborting");
    abort();
  }

  return Exit::success;
}

