/*
 * mpeg - MPEG frame header structures - implementation
 * Copyright(c) 2004 of wave++ (Yuri D'Elia) <wavexx@users.sf.net>
 * Distributed under GNU LGPL without ANY warranty.
 */

// local headers
#include "mpeg.hh"

// c system headers
#include <cstring>
using std::memcpy;
using std::memchr;


namespace mpeg
{
  // tables
  const int bitrates[5][16] =
  {
    {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0},
    {0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0},
    {0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0},
    {0, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0},
    {0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0}
  };

  const int freqs[3][4] =
  {
    {44100, 48000, 32000, 0},
    {22050, 24000, 16000, 0},
    {11025, 12000,  8000, 0}
  };


  // implementation
  size_t
  check_frame(const char* const start, const size_t len)
  {
    // final frame length
    size_t frmlen(sizeof(frame_t));
    if(len < frmlen)
      return 0;
    
    // frame data
    frame_t frame;
    memcpy(&frame, start, frmlen);
    version_t version(static_cast<version_t>(frame.version));
    layer_t layer(static_cast<layer_t>(frame.layer));
    
    // check for basic data
    if((frame.sync != 0x7FF) ||
	(version == version_invalid) ||
	(layer == layer_reserved) ||
	(frame.rate == bitrate_invalid) ||
	(frame.freq == freq_invalid) ||
	(frame.emphasis == emph_reserved))
      return 0;
    
    // check for frequency/sampling rate values
    int rate(mpeg::rate(version, layer, frame.rate));
    int freq(mpeg::freq(version, layer, frame.freq));
    if(!rate || !freq)
      return 0;
    
    // final frame length
    frmlen = ((frame.layer == layer1?
		  ((12 * rate / freq + frame.pad) * 4):
		  (144 * rate / freq + frame.pad)));
    if(len < frmlen)
      return 0;
    
    // TODO: crc checking
    if(!frame.crc) {}
    
    return frmlen;
  }


  size_t
  check_frames(const char* const start, const size_t len, size_t n)
  {
    size_t pos(0);

    while(n--)
    {
      size_t frmlen;
      if(!(frmlen = check_frame(start + pos, len - pos)))
	return 0;
      pos += frmlen;
    }

    return pos;
  }


  size_t
  sync_forward(const char* const start, const size_t len, const size_t min)
  {
    const char* pos(start);
    size_t rem(len);

    while((pos = reinterpret_cast<const char*>(memchr(pos, 0xFF, rem))))
    {
      rem = len - (pos - start);
      if(check_frames(pos, rem, min))
	return (pos - start);
    }

    return len;
  }


  const void*
  memrchr(const void* start, int c, size_t len)
  {
    const char* end(reinterpret_cast<const char*>(start) + len);

    while(--end, len--)
    {
      if(*end == c)
	return end;
    }

    return NULL;
  }


  size_t
  sync_reverse(const char* const start, const size_t len, const size_t min)
  {
    const char* pos;
    size_t rem(len);
    size_t frmlen;

    while((pos = reinterpret_cast<const char*>(memrchr(start, 0xFF, rem))))
    {
      rem = (pos - start);
      if((frmlen = check_frames(pos, len - rem, min)))
	return (rem + frmlen);
    }

    return 0;
  }
}
