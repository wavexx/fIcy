/*
 * HTTP/1.0 for streams sockets - implementation
 * Copyright(c) 2003-2004 of wave++ (Yuri D'Elia)
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
using std::string;
using std::getline;

#include <sstream>
using std::istringstream;


// c system headers
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


// implementation
namespace Http
{
  Http::Http(const char* host, const int port)
  {
    this->host = strdup(host);
    this->port = (port? port: getSrvPort());
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

    return ntohs(se->s_port);
  }


  void
  Http::readReply(Socket& s, Reply& reply)
  {
    char proto[Proto::hdrLen];
    *proto = 0;

    char buf[Proto::hdrLen];
    size_t answLen;

    // read all lines
    while((answLen = s.gets(buf, Proto::hdrLen)) > Proto::endlSz)
    {
      if(reply.headers || !*proto)
      {
        // cut tailing newlines from the buffer
        while(strchr(Proto::endl, buf[answLen - 1]))
          buf[--answLen] = 0;

	if(!*proto)
	  memcpy(proto, buf, answLen);
	else if(reply.headers)
	  reply.headers->push_back(string(buf, answLen));
      }
    }

    // parse the first protocol line
    istringstream in(proto);
    in >> reply.proto;
    in >> reply.code;
    in >> std::ws;
    getline(in, reply.description);
  }


  Socket*
  Http::gen(const char* act, const char* path,
      Reply& reply, const Header* headers)
  {
    using std::auto_ptr;
    using std::string;

    auto_ptr<Socket> s(new Socket(addr, port));

    // request
    string req;
    req = string(act) + " " + urlEncode(path) + " " +
      Proto::version + Proto::endl;
    req += string("Host: ") + host + Proto::endl;

    // headers
    if(headers)
      for(Header::const_iterator it = headers->begin();
          it != headers->end(); ++it)
	req += *it + Proto::endl;

    // final newline
    req += Proto::endl;
    s->write(req.c_str(), req.size());

    // fetch the answer headers
    readReply(*s, reply);

    return s.release();
  }


  Socket*
  Http::get(const char* file, Reply& reply, const Header* headers)
  {
    // we used to close the write end here, as HTTP/1.0 requests really end
    // here and we're very happy to relase some resources. Unfortunately,
    // SHOUTcast > 1.9 servers simply interrupt the connection when receiving
    // the notification. "We fixed all known bugs in 1.9". Aisee...
    return gen(Proto::get, file, reply, headers);
  }
}
