/*
 * mpeg - MPEG frame header structures
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

#ifndef mpeg_hh
#define mpeg_hh

// system headers
#include <cstddef>
#include <inttypes.h>


namespace mpeg
{
  // imports
  using std::size_t;


  // types
  enum version_t
  {
    version25 = 0,
    version_invalid = 1,
    version2 = 2,
    version1 = 3
  };

  enum layer_t
  {
    layer_reserved = 0,
    layer3 = 1,
    layer2 = 2,
    layer1 = 3
  };

  enum emph_t
  {
    emph_none = 0,
    emph_5015 = 1,
    emph_reserved = 2,
    emph_ccitj17 = 3
  };


  // frame header
  struct frame_t
  {
    bool sync;
    version_t ver;
    layer_t layer;
    bool crc;
    unsigned rate;
    unsigned freq;
    bool pad;
    unsigned mode;
    unsigned ext;
    unsigned copy;
    emph_t emph;
  };


  // bitrate tables
  const unsigned bitrate_invalid = 0x0F;
  extern const int bitrates[5][16];

  inline int
  rate(version_t version, layer_t layer, int v)
  {
    return 1000 * bitrates[
	(version == version1? 3 - layer: 3 + (layer == layer1? 0: 1))][v];
  }


  // frequency tables
  const unsigned freq_invalid = 0x03;
  extern const int freqs[3][4];

  inline int
  freq(version_t version, layer_t layer, int v)
  {
    return freqs[3 - (version == version25? 1: version)][v];
  }


  // frame check and sync
  size_t
  check_frame(const char* const start, const size_t len);

  size_t
  check_frames(const char* const start, const size_t len, size_t n);

  size_t
  sync_forward(const char* const start, const size_t len, const size_t min);

  size_t
  sync_reverse(const char* const start, const size_t len, const size_t min);
}

#endif
