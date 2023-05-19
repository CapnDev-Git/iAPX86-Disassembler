#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048 // 2KB buffer

void hexdump(const char *path, unsigned char *buffer, size_t *buffer_size,
             size_t *program_size);