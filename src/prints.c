#include "prints.h"

void print_headers(char *format, size_t byte_len, size_t line_len) {
  printf(ADR_HEADER SPACE2);
  size_t i, j;
  for (i = 0; i < line_len; i++) {
    if (i % (line_len / 2) == 0 && i != 0)
      printf(SPACE2);
    printf(format, i);
  }
  printf("\n");

  printf(ADR_SEPERATOR);

  printf(SPACE);
  for (i = 0; i < line_len; i++) {
    if (i % (line_len / 2) == 0 && i != 0)
      printf(SPACE2);
    printf(SPACE);
    for (j = 0; j < byte_len; j++)
      printf("-");
  }

  printf("\n");
}

void print_hexdump(const unsigned char *buffer, size_t buffer_size) {
  print_headers(HEX_FORMAT_U, 2, HEX_PER_LINE);

  size_t i, j;
  for (i = 0; i < buffer_size; i += HEX_PER_LINE) {
    printf(ADDRESS_FORMAT, i);

    for (j = 0; j < HEX_PER_LINE && (i + j) < buffer_size; j++) {
      if (j % (HEX_PER_LINE / 2) == 0 && j != 0)
        printf(SPACE);
      printf(HEX_FORMAT_L, (unsigned char)buffer[i + j]);
    }

    if (j < HEX_PER_LINE) {
      size_t padding_size = (HEX_PER_LINE - j) * 3 + 2;
      printf("%*s", (int)padding_size, "");
    }

    printf(SPACE ASCII_SEPARATOR);
    for (j = 0; j < HEX_PER_LINE && (i + j) < buffer_size; j++) {
      unsigned char c = buffer[i + j];
      printf("%c", isprint(c) ? c : NO_CHAR);
    }

    printf(ASCII_SEPARATOR "\n");
  }

  printf(ADDRESS_FORMAT "\n\n", i - HEX_PER_LINE + j);
}

/// @param size in BYTES
void printb(const unsigned char *bin_str, size_t size, size_t byte_per_line) {
  print_headers(BIN_FORMAT, 8, byte_per_line);

  size_t i, j;
  for (i = 0; i < size; i += byte_per_line) {
    if (size > byte_per_line)
      printf(ADDRESS_FORMAT, i);

    for (j = 0; j < byte_per_line && (i + j) < size; j++) {
      if (j % (byte_per_line / 2) == 0 && j != 0)
        printf(SPACE2);
      printf(BYTE_TO_BINARY_PATTERN SPACE, BYTE_TO_BINARY(bin_str[i + j]));
    }

    if (j < byte_per_line) {
      size_t padding_size = (byte_per_line - j) * 9 + 2;
      printf("%*s", (int)padding_size, "");
    }

    printf(SPACE ASCII_SEPARATOR);
    for (j = 0; j < byte_per_line && (i + j) < size; j++) {
      unsigned char c = bin_str[i + j];
      printf("%c", isprint(c) ? c : NO_CHAR);
    }

    printf(ASCII_SEPARATOR "\n");
  }

  printf(ADDRESS_FORMAT "\n\n", i - HEX_PER_LINE + j);
}

/// @param a address
/// @param il instruction length in bytes
/// @param ip instruction pointer
void print4b(const unsigned char *p, size_t a, size_t il, size_t *ip) {
  printf(DIS_ADDRESS_FORMAT, a);
  for (size_t j = 0; j < il; j++) {
    printf(INSTRUCTION_FORMAT, p[a + j]);
  }

  switch (il) {
  case 1:
    printf("%s", IS1);
    break;
  case 2:
    printf("%s", IS2);
    break;
  case 3:
    printf("%s", IS3);
    break;
  case 4:
    printf("%s", IS4);
    break;
  case 5:
    printf("%s", IS5);
    break;
  case 6:
    printf("%s", IS6);
    break;
  }

  *ip += il;
}

void printOK(const char *filename) {
  printf("\033[1;32mOK\033[0m %s\n", filename);
}

void printKO(const char *filename) {
  printf("\033[1;31mKO\033[0m %s\n", filename);
}

void printDiff(const char *errorLine, const char *ref) {
  const char *lineStart = errorLine;
  const char *lineEnd = errorLine;
  while (lineStart > errorLine - 50 && *lineStart != '\n')
    --lineStart;
  while (*lineEnd != '\0' && *lineEnd != '\n')
    ++lineEnd;

  printf("\033[0m");
  for (const char *ch = lineStart; ch < errorLine; ++ch)
    putchar(*ch);

  printf("\033[1;31m");
  putchar(*errorLine);

  for (const char *ch = errorLine + 1; ch < lineEnd; ++ch)
    putchar(*ch);
  printf("\033[0m");

  lineStart = ref;
  lineEnd = ref;
  while (lineStart > ref - 50 && *lineStart != '\n')
    --lineStart;
  while (*lineEnd != '\0' && *lineEnd != '\n')
    ++lineEnd;

  printf("\033[0m");
  for (const char *ch = lineStart; ch < ref; ++ch) {
    putchar(*ch);
  }

  printf("\033[1;32m");
  putchar(*ref);
  for (const char *ch = ref + 1; ch < lineEnd; ++ch) {
    putchar(*ch);
  }

  printf("\033[0m\n");
}