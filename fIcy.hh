/*
 * fIcy - HTTP/1.0-ICY stream extractor/separator
 * Copyright(c) 2003-2004 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef fIcy_hh
#define fIcy_hh

// system headers (for size_t)
#include <locale>

// some defines
#define FICY_VERSION "1.0.8beta"


// some constants
namespace fIcy
{
  // common strings
  const char version[] = FICY_VERSION;
  const char copyright[] =
    "Copyright(c) 2003-2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>\n"
    "Distributed under GNU LGPL (v2 or above) without ANY warranty.\n";


  // fIcy defaults
  const char userAgent[] = "User-agent: fIcy " FICY_VERSION;
  const size_t bufSz = 1024;
  
  const char fIcyHelp[] =
    " [-dosqxXemvtcinprh] <server [port [path]]|url>\n\n"
    "  -d\tDo not dup the output to stdout\n"
    "  -o file\tDump the output to file (or use file as a prefix)\n"
    "  -s sfx\tUse sfx as a suffix for new files\n"
    "  -q file\tAppend file sequence list to file (only when saving)\n"
    "  -x regex\tDump only titles matching regex\n"
    "  -X regex\tDo NOT dump titles matching regex\n"
    "  -e\tEnumerate files when song metadata changes. Use file as a prefix\n"
    "  -m\tUse song metadata when writing filenames. file used as a prefix\n"
    "  -v\tVerbose\n"
    "  -t\tDisplay metadata while downloading\n"
    "  -c\tDo not clobber files (implicit with -n)\n"
    "  -i\tAbort when clobbering is attempted (implies -c)\n"
    "  -n\tIf the file exists create a new file with .n appended\n"
    "  -p\tWhen duping consider writing errors as transient\n"
    "  -r\tRemove partials. Keep only complete sequences\n\n";


  // fResync defaults
  const size_t frameLen = 1597; // (114 * 448000 / 32000 + 1)
  const size_t maxFrames = 6;

  const char fResyncHelp[] =
    " [-shv] [-n frames] [-m len] file\n\n"
    "  -s\tSimulate only (print frame sync offsets: start and size)\n"
    "  -v\tIncrement verbosity level\n"
    "  -n frames\tNumber of frames to check\n"
    "  -m len\tMaximum frame length\n\n";

  
  // fPls defaults
  const size_t maxRetries = 1;
  const size_t maxLoops = 1;
  const size_t waitSecs = 3;

  const char fPlsHelp[] =
    " [-PRLTvh] <file|url> [fIcy options]\n\n"
    "  -v\tVerbose\n"
    "  -P path\tSpecify fIcy executable name/path\n"
    "  -R max\tMax per-stream retries\n"
    "  -L max\tMax playlist loops\n"
    "  -T sec\tWait sec seconds after each failure\n\n";
}


namespace Exit
{
  const int success = 0;
  const int fail = 1;
  const int args = 2;
}

#endif
