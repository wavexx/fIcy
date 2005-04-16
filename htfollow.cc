/*
 * htFollow - follow an http url until content is reached - implementation
 * Copyright(c) 2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

// interface and local headers
#include "htfollow.hh"
#include "hdrparse.hh"
#include "sanitize.hh"
#include "msg.hh"
using std::string;
using std::map;

// system headers
#include <memory>
using std::auto_ptr;

#include <stdexcept>
using std::runtime_error;


// implementation
string
itos(const int i)
{
  char buf[16];
  snprintf(buf, sizeof(buf), "%d", i);
  return string(buf);
}


Socket*
htFollow(map<string, string>& pReply, const URL& url,
    const Http::Header qHeaders, size_t limit)
{
  URL buf = url;

  // connection loop
  auto_ptr<Socket> s;
  for(;; --limit)
  {
    msg("connecting to (%s %d)", buf.server.c_str(), buf.port);
    Http::Http httpc(buf.server.c_str(), buf.port);

    msg("requesting data on (%s)", buf.path.c_str());
    Http::Header aHeaders;
    Http::Reply reply(&aHeaders);
    s.reset(httpc.get(buf.path.c_str(), reply, &qHeaders));

    // validate the reply code
    if(reply.code != Http::Proto::ok &&
	reply.code != Http::Proto::found &&
	reply.code != Http::Proto::other)
      throw runtime_error(string("unexpected reply: ") +
	  itos(reply.code) + " " + sanitize_esc((reply.description.size()?
		  reply.description: reply.proto)).c_str());

    // parse the headers
    pReply = Http::hdrParse(aHeaders);
    if(reply.code == Http::Proto::ok)
      break;

    // recursion
    if(!limit)
      throw runtime_error(string("hit redirect follow limit: ") + itos(limit));

    map<string, string>::iterator urlPos = pReply.find(Http::Proto::location);
    if(urlPos == pReply.end())
      throw runtime_error("redirection didn't contain an url");

    buf = urlPos->second;
    if(buf.proto != url.proto)
      throw runtime_error("protocol changes are not allowed");
  }

  return s.release();
}
