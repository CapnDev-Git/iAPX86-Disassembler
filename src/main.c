#include "main.h"

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3)
    errx(1, "Usage: %s <flags> <file>", argv[0]);

  if (argc == 2 && (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--dump")))
    errx(1, "Usage: %s -d <file>", argv[0]);

  if (argc == 3 && argv[1][0] != '-')
    errx(1, "Provided flag not properly formatted: %s", argv[1]);

  if (argc == 3 && strcmp(argv[1], "-d") && strcmp(argv[1], "--dump"))
    errx(1, "Unknown flag: %s", argv[1]);

  // Initialize the buffer
  unsigned char *buffer = malloc(BUFFER_SIZE * sizeof(unsigned char));
  if (buffer == NULL)
    errx(1, "Can't allocate memory for the buffer!");

  // Get the hexadecimal dump of the file
  size_t buffer_size, program_size;
  hexdump(argv[argc - 1], buffer, &buffer_size, &program_size);

  unsigned char *program = malloc(program_size * 2 * sizeof(unsigned char));
  if (program == NULL)
    errx(1, "Can't allocate memory for the text area!");

  // Copy the text area to the program buffer
  memcpy(program, buffer + HEADER_SIZE, program_size);

  // Print according to the number of arguments
  if (argc == 2) {
    // Translate the binary buffer to 8086 assembly
    translate_bin(program, program_size);
  } else {
    // Print the hexadecimal dump & the text area
    print_hexdump(buffer, buffer_size);
    // printb(program, program_size, BYTE_PER_LINE);
    printf("Text-area size: 0x%08lx = %zu bytes\n", program_size, program_size);
  }

  // Free & exit
  free(buffer);
  free(program);
  return 0;
}