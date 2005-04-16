/*
 * htFollow - follow an http url until content is reached
 * Copyright(c) 2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef htfollow_hh
#define htfollow_hh

// local headers
#include "urlparse.hh"
#include "http.hh"

// system headers
#include <map>


Socket*
htFollow(std::map<std::string, std::string>& pReply, const URL& url,
    const Http::Header qHeaders, size_t limit = 0);

#endif
