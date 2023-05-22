#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 131072 // 128 KiB

void hexdump(const char *path, unsigned char *buffer, size_t *buffer_size,
             size_t *program_size);