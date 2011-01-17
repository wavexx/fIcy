/*
 * fResync - MPEG frame-resync utility
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "fIcy.hh"
#include "msg.hh"
#include "mpeg.hh"
#include "copy.hh"

// system headers
#include <fstream>
using std::ifstream;
using std::ofstream;

#include <iostream>
using std::cout;
using std::cerr;

#include <memory>
using std::auto_ptr;

// c system headers
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
 * Parameters handling
 */
struct params_t
{
  const char* file;
  size_t framelen;
  size_t maxframes;
  bool buffered;
  bool simulate;
  bool help;
};


bool
parse_params(params_t& buf, int argc, char* const argv[])
{
  // real values
  int arg;
  while((arg = getopt(argc, argv, "bn:m:svh")) != -1)
    switch(arg)
    {
    case 'b':
      buf.buffered = true;
      break;

    case 'n':
      buf.maxframes = strtoul(optarg, NULL, 0);
      break;

    case 'm':
      buf.framelen = strtoul(optarg, NULL, 0);
      break;

    case 's':
      buf.simulate = true;
      break;

    case 'h':
      buf.help = true;
      break;

    case 'v':
      verbose = true;
      break;
    }

  argc -= optind;
  buf.file = argv[optind];

  return (!buf.file);
}


void
show_help()
{
  cout << prg << fIcy::fResyncHelp << prg << " v" << fIcy::version <<
    " is\n" << fIcy::copyright;
}


bool
init(params_t& buf, int argc, char* argv[])
{
  // defaults
  prg = argv[0];
  buf.buffered = false;
  buf.framelen = fIcy::frameLen;
  buf.maxframes = fIcy::maxFrames;
  verbose = buf.simulate = buf.help = false;

  // arguments
  bool ret(parse_params(buf, argc, argv));
  return (ret && !buf.help);
}


/*
 * File truncation utilities
 */
struct region_t
{
  size_t start;
  size_t size;
};


bool
truncate(const char* file, const region_t& reg)
{
  // check whether truncate can be used
  if(reg.start == 0)
    return truncate(file, reg.size);

  // re-open the input file and output file
  ifstream in(file);
  if(!in || unlink(file))
    return true;
  ofstream out(file);
  if(!out)
    return true;

  // skip garbage and copy the stream
  in.seekg(reg.start);
  return copy(out, in, reg.size);
}


/*
 * Implementation
 */
bool
search_sync(ifstream& fd, const params_t& params, region_t& reg)
{
  // create the buffer as the double of the maximal space possibly needed
  size_t mlen(params.framelen * params.maxframes * 2);
  auto_ptr<char> buf(new char[mlen]);

  // beginning
  fd.seekg(0);
  fd.read(buf.get(), mlen);
  if(!fd)
    return true;

  reg.start = mpeg::sync_forward(buf.get(), mlen, params.maxframes);
  if(reg.start == mlen)
    return true;

  // end
  fd.seekg(-static_cast<ifstream::off_type>(mlen), std::ios_base::end);
  size_t pos(fd.tellg());
  fd.read(buf.get(), mlen);
  if(!fd)
    return true;

  size_t end(mpeg::sync_reverse(buf.get(), mlen, params.maxframes));
  if(!end || (pos + end) <= reg.start)
    return true;
  reg.size = (pos + end) - reg.start;

  return false;
}


bool
search_sync(const char* buf, size_t len, const params_t& params, region_t& reg)
{
  // create the buffer as the double of the maximal space possibly needed
  size_t mlen(params.framelen * params.maxframes * 2);
  if(len < mlen)
    mlen = len;

  // beginning
  reg.start = mpeg::sync_forward(buf, mlen, params.maxframes);
  if(reg.start == mlen)
    return true;

  // end
  size_t pos(len - mlen);
  size_t end(mpeg::sync_reverse(buf + pos, mlen, params.maxframes));
  if(!end || (pos + end) <= reg.start)
    return true;
  reg.size = (pos + end) - reg.start;

  return false;
}


int
resync_buf(const params_t& params)
{
  // open the file
  ifstream fd(params.file, (params.simulate?
	  std::ios_base::in: (std::ios_base::in | std::ios_base::out)));
  if(!fd)
  {
    err("cannot open %s for read/write", params.file);
    return Exit::fail;
  }

  // search the offsets
  region_t reg;
  if(search_sync(fd, params, reg))
  {
    err("cannot resync %s, try increasing frame size", params.file);
    return Exit::fail;
  }
  msg("sync found at %lu for %lu bytes", reg.start, reg.size);

  // save file size
  fd.seekg(0, std::ios_base::end);
  size_t size(fd.tellg());
  fd.close();

  // sync the file
  if(params.simulate)
  {
    // offsets only
    cout << reg.start << " " << reg.size << std::endl;
  }
  else if(reg.start != 0 || reg.size != size)
  {
    // the file needs to be modified
    if(truncate(params.file, reg))
    {
      err("cannot truncate %s", params.file);
      return Exit::fail;
    }
  }
  else
    msg("no resync needed");

  return Exit::success;
}


int
resync_mmap(const params_t& params)
{
  // open the file
  int fd = open(params.file, O_RDWR);
  if(fd == -1)
  {
    err("cannot open %s for read/write", params.file);
    return Exit::fail;
  }

  // file size
  struct stat st;
  if(fstat(fd, &st))
  {
    err("cannot stat %s", params.file);
    return Exit::fail;
  }

  // mmap the file
  char* addr = reinterpret_cast<char*>(mmap(NULL, st.st_size,
	  (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0));
  if(!addr)
  {
    close(fd);
    err("cannot mmap %s", params.file);
    return Exit::fail;
  }
  
  // search the offsets
  bool fail;
  region_t reg;
  if((fail = search_sync(addr, st.st_size, params, reg)))
    err("cannot resync %s, try increasing frame size", params.file);
  else
  {
    msg("sync found at %lu for %lu bytes", reg.start, reg.size);

    if(reg.start != 0 || reg.size != static_cast<size_t>(st.st_size))
    {
      // non-zero offsets
      if(reg.start != 0)
      {
	memmove(addr, addr + reg.start, reg.size);
	reg.start = 0;
      }
      
      // final truncation
      if((fail = ftruncate(fd, reg.size)))
	err("cannot truncate %s", params.file);
    }
    else
      msg("no resync needed");
  }

  // free resources
  munmap(addr, st.st_size);
  close(fd);

  return (fail? Exit::fail: Exit::success);
}


int
main(int argc, char* argv[])
{
  // initialize
  params_t params;
  if(init(params, argc, argv))
  {
    err("bad parameters; see %s -h", prg);
    return Exit::args;
  }
  if(params.help)
  {
    show_help();
    return Exit::args;
  }

  return (params.buffered || params.simulate?
      resync_buf(params):
      resync_mmap(params));
}
