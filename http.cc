/*
 * HTTP/1.0 for streams sockets - implementation
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface
#include "http.hh"

// local headers
#include "resolver.hh"
#include "urlencode.hh"

// system headers
#include <stdexcept>
#include <memory>
#include <string>

// c system headers
#include <netdb.h>
#include <string.h>


// implementation
namespace Http
{
  Http::Http(const char* host, const int port)
  {
    this->host = strdup(host);
    this->port = port? port: getSrvPort();
    addr = resolve(host);
  }


  Http::~Http()
  {
    delete []host;
  }


  int
  Http::getSrvPort()
  {
    servent* se(getservbyname(Proto::proto, Proto::protoTy));
    if(!se)
      throw
        std::runtime_error("error while trying to identify http port number");

    return se->s_port;
  }


  Socket*
  Http::gen(const char* act, const char* path, const Header* headers,
      Header* reply)
  {
    using std::auto_ptr;
    using std::string;

    auto_ptr<Socket> s(new Socket(addr, port));

    // request
    string req;
    req = string(act) + " " + urlEncode(path) + " " +
      Proto::version + Proto::endl;
    s->write(req.c_str(), req.size());

    // host name
    req = string("Host: ") + host + Proto::endl;
    s->write(req.c_str(), req.size());

    // headers
    if(headers)
      for(Header::const_iterator it = headers->begin();
          it != headers->end(); ++it)
      {
        s->write(it->c_str(), it->size());
        s->write(Proto::endl, Proto::endlSz);
      }

    // final newline
    s->write(Proto::endl, Proto::endlSz);

    // fetch the answer headers
    char buf[Proto::hdrLen];
    size_t answLen;
    while((answLen = s->gets(buf, Proto::hdrLen)) > Proto::endlSz)
    {
      if(reply)
      {
        // cut tailing newlines from the buffer
        while(strchr(Proto::endl, buf[answLen - 1]))
          buf[--answLen] = 0;

        reply->push_back(buf);
      }
    }

    return s.release();
  }


  Socket*
  Http::get(const char* file, const Header* headers, Header* reply)
  {
    using std::auto_ptr;
    auto_ptr<Socket> s(gen(Proto::get, file, headers, reply));

    // close the write end
    s->close(SHUT_WR);

    return s.release();
  }
}

