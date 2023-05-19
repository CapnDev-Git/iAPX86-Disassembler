#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX_PER_LINE 16              // 16 bytes per line when printing
#define BYTE_PER_LINE 4              // 8 bytes per line when printing
#define HEX_FORMAT_U "%02X "         // Format for printing hex
#define HEX_FORMAT_L "%02x "         // Format for printing hex
#define BIN_FORMAT "%08X "           // Format for printing binary
#define ADDRESS_FORMAT "%08zx  "     // Format for printing address
#define DIS_ADDRESS_FORMAT "%04lx: " // Format for printing disassembler address
#define INSTRUCTION_FORMAT "%02x"    // Format for printing hex
#define NO_CHAR '.'                  // Character to print if not printable
#define ASCII_SEPARATOR "|"

#define ADR_SEPERATOR "--------" // Separator between address and hex
#define ADR_HEADER "address "    // Header for address column

#define IS1 "            " // Instruction spacing for 1 byte
#define IS2 "          "   // Instruction spacing for 2 bytes
#define IS3 "        "     // Instruction spacing for 3 bytes
#define IS4 "      "       // Instruction spacing for 4 bytes
#define IS5 "    "         // Instruction spacing for 5 bytes
#define SPACE " "          // Regular space (for dump printing)
#define SPACE2 "  "        // Double space (for dump printing)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte)&0x80 ? '1' : '0'), ((byte)&0x40 ? '1' : '0'),                        \
      ((byte)&0x20 ? '1' : '0'), ((byte)&0x10 ? '1' : '0'),                    \
      ((byte)&0x08 ? '1' : '0'), ((byte)&0x04 ? '1' : '0'),                    \
      ((byte)&0x02 ? '1' : '0'), ((byte)&0x01 ? '1' : '0')

void print_headers(char *format, size_t byte_len, size_t line_len);
void print_hexdump(const unsigned char *buffer, size_t buffer_size);
void printb(const unsigned char *bin_str, size_t size, size_t byte_per_line);
void print4b(const unsigned char *p, size_t a, size_t il, size_t *ip);