/*
 * urlParse - parse an url in proto/server/port/path components
 * Copyright(c) 2004 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef urlparse_hh
#define urlparse_hh

// system headers
#include <string>


void
urlParse(std::string& proto, std::string& server, int& port, std::string& path,
    const std::string& url);

#endif
