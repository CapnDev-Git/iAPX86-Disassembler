#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dump.h"
#include "prints.h"
#include "translate.h"

#define HEADER_PROGRAM_SIZE 8 // Program size starting at byte 8 (len=4 bytes)
#define HEADER_SIZE 32        // Bytes 0-31 are the header

int main(int argc, char **argv);