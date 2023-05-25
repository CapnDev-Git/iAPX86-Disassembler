#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "patterns.h"
#include "prints.h"

#define EA_STRING_SIZE 20 // Chosen arbitrarily

void get_adm(const unsigned char *p, size_t a, unsigned char mod,
             unsigned char rm, char *ea, size_t *ds);

void translate_bin(const unsigned char *p, size_t p_size);