/*
 * plsParse - M3U/EXTM3U/PLS/PLSv2 playlist parser/s
 * Copyright(c) 2004 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef plsparse_hh
#define plsparse_hh

// system headers
#include <string>
#include <istream>
#include <list>


/*
 * Specific parsers
 */
void
m3uParse(std::list<std::string>& list, std::istream& fd);

void
extm3uParse(std::list<std::string>& list, std::istream& fd);

void
plsv2Parse(std::list<std::string>& list, std::istream& fd);


/*
 * Identify automatically the playlist type and parse it
 */
void
plsParse(std::list<std::string>& list, std::istream& fd);

#endif
