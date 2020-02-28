// crc.h            see license.txt for copyright and terms of use
// simple crc function

#ifndef __CRC_H
#define __CRC_H

#ifndef _MSC_VER
  unsigned long crc32(unsigned char const *data, int length);
#else
  uint32_t crc32(unsigned char const *data, int length);
#endif

#endif // __CRC_H
