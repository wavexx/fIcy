/*
 * sanitize - rewrite a string to a suitable (secure) one
 * Copyright(c) 2003-2007 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef sanitize_hh
#define sanitize_hh

// system headers
#include <string>


/*
 * Rewrite a file name component, without path, replacing invalid
 * characters with an approximation
 */
std::string
sanitize_file(const std::string& src);

// Rewrite a string without escape sequences
std::string
sanitize_esc(const std::string& src);

#endif
