#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024      // 1KB buffer
#define HEADER_PROGRAM_SIZE 8 // Program size starting at byte 8 (4 bytes)
#define HEADER_SIZE 32        // Bytes 0-31 are the header
#define HEX_PER_LINE 16       // 16 bytes per line when printing
#define BYTE_PER_LINE 4       // 8 bytes per line when printing

#define XFORMAT "%02X "      // Format for printing hex
#define BFORMAT "%08X "      // Format for printing binary
#define ASMFORMAT "      \t" // Format for printing assembly

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte)&0x80 ? '1' : '0'), ((byte)&0x40 ? '1' : '0'),                        \
      ((byte)&0x20 ? '1' : '0'), ((byte)&0x10 ? '1' : '0'),                    \
      ((byte)&0x08 ? '1' : '0'), ((byte)&0x04 ? '1' : '0'),                    \
      ((byte)&0x02 ? '1' : '0'), ((byte)&0x01 ? '1' : '0')