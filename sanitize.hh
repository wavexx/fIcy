/*
 * sanitize - rewrite a string to a suitable (secure) filename
 * Copyright(c) 2003 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef sanitize_hh
#define sanitize_hh

// system headers
#include <string>


std::string
sanitize(const std::string& src);

#endif

