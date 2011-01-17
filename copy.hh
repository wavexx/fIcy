/*
 * copy - stream 2 stream buffered copy
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef copy_hh
#define copy_hh

// system headers
#include <iostream>

// c system headers
#include <stdio.h>


bool
copy(std::ostream& out, std::istream& in, const std::size_t& len,
    const std::size_t& bufSz = BUFSIZ);

#endif
