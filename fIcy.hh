/*
 * fIcy - HTTP/1.0-ICY stream extractor/separator
 * Copyright(c) 2003-2005 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef fIcy_hh
#define fIcy_hh

// system headers (for size_t)
#include <locale>

// some defines
#define FICY_VERSION "1.0.14beta"


// some constants
namespace fIcy
{
  // common strings
  const char version[] = FICY_VERSION;
  const char copyright[] =
    "Copyright(c) 2003-2005 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>\n"
    "Distributed under GNU LGPL (v2 or above) without ANY warranty.\n";


  // fIcy defaults
  const char userAgent[] = "User-agent: fIcy " FICY_VERSION;
  const size_t bufSz = 1024;
  const char sed[] = "sed";
  const size_t maxFollow = 1;
  
  const char fIcyHelp[] =
    " [-dosqxXIfFMialemvtcnprh] <server [port [path]]|url>\n\n"
    "  -d\t\tDo not dump the output to stdout\n"
    "  -o file\tDump the output to file (or use file as a prefix)\n"
    "  -s sfx\tUse sfx as a suffix for new files\n"
    "  -q file\tAppend file sequence list to file (only when saving)\n"
    "  -x regex\tDump only titles matching regex\n"
    "  -X regex\tDo NOT dump titles matching regex\n"
    "  -I file\tLoad include/exclude regexs from file\n"
    "  -f expr\tRewrite titles using the specified sed expression\n"
    "  -F file\tRewrite titles using the specified sed script\n"
    "  -M time\tMaximum playing time\n"
    "  -i time\tMaximum network idle time\n"
    "  -a file\tProvide HTTP credentials (user:pass file)\n"
    "  -l num\tRedirect follow limit\n"
    "  -e\t\tEnumerate files when song metadata changes. Use file as prefix\n"
    "  -E num\tEnumeration starting number (0 autodetects). Implies -e\n"
    "  -m\t\tUse song metadata when writing filenames. file used as prefix\n"
    "  -v\t\tVerbose\n"
    "  -t\t\tDisplay metadata while downloading\n"
    "  -c\t\tDo not clobber files (implicit with -n)\n"
    "  -n\t\tIf the file exists create a new file with .n appended\n"
    "  -p\t\tWhen dumping consider writing errors as transient\n"
    "  -r\t\tRemove partials. Keep only complete sequences\n\n";


  // fResync defaults
  const size_t frameLen = 1597; // (114 * 448000 / 32000 + 1)
  const size_t maxFrames = 6;

  const char fResyncHelp[] =
    " [-bshv] [-n frames] [-m len] file\n\n"
    "  -b\t\tRevert to buffered mode (implicit with -s)\n"
    "  -s\t\tSimulate only (print frame sync offsets: start and size)\n"
    "  -v\t\tIncrement verbosity level\n"
    "  -n frames\tNumber of frames to check\n"
    "  -m len\tMaximum frame length\n\n";

  
  // fPls defaults
  const size_t maxRetries = 1;
  const size_t maxLoops = 1;
  const size_t waitSecs = 15;

  const char fPlsHelp[] =
    " [-PRLTMaldvh] <file|url> [fIcy options]\n\n"
    "  -v\t\tVerbose\n"
    "  -P path\tSpecify fIcy executable name/path\n"
    "  -R max\tMax per-stream retries\n"
    "  -L max\tMax playlist loops\n"
    "  -T time\tWait the specified time after each failure\n"
    "  -M time\tMaximum cumulative playing time\n"
    "  -l num\tRedirect follow limit\n"
    "  -a file\tProvide HTTP credentials (user:pass file)\n"
    "  -d file\tRun as a daemon, redirecting messages to file\n\n";
}


namespace Exit
{
  const int success = 0;
  const int fail = 1;
  const int args = 2;
}

#endif
