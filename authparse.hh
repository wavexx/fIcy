/*
 * authparse - parse/get HTTP credentials
 * Copyright(c) 2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef authparse_hh
#define authparse_hh

// local headers
#include "http.hh"


void
authParse(Http::Auth& out, const char* file);

#endif
