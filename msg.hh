/*
 * msg - user messages
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef msg_hh
#define msg_hh

// global parameters
extern const char* prg;
extern bool verbose;


// error message (always visible)
void
err(const char* fmt, ...);

// verbose message
void
msg(const char* fmt, ...);

#endif
