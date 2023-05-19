#include "dump.h"

void hexdump(const char *path, unsigned char *buffer, size_t *buffer_size,
             size_t *program_size) {
  FILE *pfile;
  pfile = fopen(path, "rb");
  if (pfile == NULL)
    errx(1, "Provided file can't be opened!");

  size_t read_bytes = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, pfile);
  if (read_bytes == 0)
    errx(1, "Provided file is empty!");

  fclose(pfile);

  memcpy(program_size, buffer + 8, 4);
  *buffer_size = read_bytes;
}