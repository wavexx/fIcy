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
#define FICY_VERSION "1.0.5beta"


// some constants
namespace fIcy
{
  const char version[] = FICY_VERSION;
  const char copyright[] =
    "Copyright(c) 2003-2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>\n"
    "Distributed under GNU LGPL (v2 or above) without ANY warranty.\n";
  const char userAgent[] = "User-agent: fIcy " FICY_VERSION;
  const size_t bufSz(1024);
  
  // an embedded help string
  const char help[] =
    " [-dosqxemvtcinprh] <server [port [path]]|url>\n\n"
    "  -d\tDo not dup the output to stdout\n"
    "  -o file\tDump the output to file (or use file as a prefix)\n"
    "  -s sfx\tUse sfx as a suffix for new files\n"
    "  -q file\tAppend file sequence list to file (only when saving)\n"
    "  -x regex\tDump only titles maching regex\n"
    "  -e\tEnumerate files when song metadata changes. Use file as a prefix\n"
    "  -m\tUse song metadata when writing filenames. file used as a prefix\n"
    "  -v\tVerbose\n"
    "  -t\tDisplay metadata while downloading\n"
    "  -c\tDo not clobber files (implicit with -n)\n"
    "  -i\tAbort when clobbering is attempted (implies -c)\n"
    "  -n\tIf the file exists create a new file with .n appended\n"
    "  -p\tWhen duping consider writing errors as transient\n"
    "  -r\tRemove partials. Keep only complete sequences\n"
    "  -h\tThis help (how cute!)\n\n";
}


namespace Exit
{
  const int success(0);
  const int fail(1);
  const int args(2);
}

#endif
