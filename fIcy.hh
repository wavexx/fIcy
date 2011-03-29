/*
 * fIcy - HTTP/1.0-ICY stream extractor/separator
 * Copyright(c) 2003-2009 of wave++ (Yuri D'Elia)
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef fIcy_hh
#define fIcy_hh

// system headers (for size_t)
#include <cstddef>

// some defines
#define FICY_VERSION "1.0.18"


// some constants
namespace fIcy
{
  // common strings
  const char version[] = FICY_VERSION;
  const char copyright[] =
    "Copyright(c) 2003-2011 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>\n"
    "Distributed under GNU LGPL (v2 or above) without ANY warranty.\n";


  // fIcy defaults
  const char userAgent[] = "User-agent: fIcy " FICY_VERSION;
  const size_t bufSz = 1024;
  const char coproc[] = "sed";
  const size_t maxFollow = 1;

  const char fIcyHelp[] =
    " [-dposqmEtxXIfFCailMcnrv] <server [port [path]]|url>\n\n"
    "  -d\t\tDo not dump the output to stdout\n"
    "  -p\t\tWhen dumping to stdout consider writing errors as transient\n"
    "  -o file\tDump the output to file (or use file as a prefix with -mE)\n"
    "  -s sfx\tUse sfx as a suffix for new files\n"
    "  -q file\tAppend \"file name\" sequence to file (only when saving)\n"
    "  -m\t\tUse song title when writing filenames. file used as prefix\n"
    "  -E num\tEnumerate written files, starting at num (0 autodetects).\n"
    "  -t\t\tDisplay titles while downloading\n"
    "  -x regex\tDump only titles matching regex\n"
    "  -X regex\tDo NOT dump titles matching regex\n"
    "  -I file\tLoad include/exclude regexs from file\n"
    "  -f expr\tRewrite titles using the specified expression\n"
    "  -F file\tRewrite titles using the specified script\n"
    "  -C path\tRewrite coprocessor (default: sed)\n"
    "  -a file\tProvide HTTP credentials (user:pass file)\n"
    "  -i time\tMaximum network idle time\n"
    "  -l num\tRedirect follow limit\n"
    "  -M time\tMaximum playing time\n"
    "  -c\t\tDo not clobber files (implicit with -n)\n"
    "  -n\t\tIf the file exists create a new file with .n appended\n"
    "  -r\t\tRemove partials. Keep only complete sequences\n"
    "  -v\t\tVerbose\n\n";


  // fResync defaults
  const size_t frameLen = 1597; // (114 * 448000 / 32000 + 1)
  const size_t maxFrames = 6;

  const char fResyncHelp[] =
    " [-bsv] [-n frames] [-m len] file\n\n"
    "  -b\t\tBuffered I/O mode (implicit with -s)\n"
    "  -s\t\tSimulate only (print frame sync offsets: start and size)\n"
    "  -v\t\tIncrement verbosity level\n"
    "  -n frames\tNumber of frames to check\n"
    "  -m len\tMaximum frame length\n\n";


  // fPls defaults
  const size_t maxRetries = 1;
  const size_t maxLoops = 1;
  const size_t waitSecs = 15;

  const char fPlsHelp[] =
    " [-LMPRTadilv] <file|url> [fIcy options]\n\n"
    "  -L max\tMaximum playlist loops\n"
    "  -M time\tMaximum cumulative playing time\n"
    "  -P path\tSpecify fIcy executable name/path\n"
    "  -R max\tMaximum per-stream retries\n"
    "  -T time\tWait the specified time after each failure\n"
    "  -a file\tProvide HTTP credentials (user:pass file)\n"
    "  -d file\tRun as a daemon, redirecting messages to file\n"
    "  -i time\tMaximum network idle time\n"
    "  -l num\tRedirect follow limit\n"
    "  -v\t\tVerbose\n\n";
}


namespace Exit
{
  const int success = 0;
  const int fail = 1;
  const int args = 2;
}

#endif
