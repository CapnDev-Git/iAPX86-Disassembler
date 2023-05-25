#include "utest.h"

char *get_cout(const char *command) {
  char *buffer = (char *)malloc(MAX_LENGTH * sizeof(char));
  if (buffer == NULL) {
    printf("Memory allocation failed. Exiting.\n");
    return NULL;
  }

  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    printf("Failed to execute command. Exiting.\n");
    free(buffer);
    return NULL;
  }

  char line[MAX_LENGTH];
  buffer[0] = '\0';
  char *newline;

  while (fgets(line, sizeof(line), fp) != NULL) {
    newline = strchr(line, '\n');
    if (newline != NULL) {
      *newline = '\0';
    }

    strncat(buffer, line, MAX_LENGTH - strlen(buffer) - 1);
    strcat(buffer, "\n");
  }

  pclose(fp);
  return buffer;
}

void cmp_files(const char *filename, int verbose) {
  char mmvmCommand[MAX_LENGTH];
  snprintf(mmvmCommand, sizeof(mmvmCommand), "mmvm -d %s 2>&1", filename);

  char *ref = get_cout(mmvmCommand);
  if (ref == NULL)
    errx(1, "Failed to execute command: %s\n", mmvmCommand);

  // Construct the command to capture the output of the disassembler
  char disassemblerCommand[MAX_LENGTH];
  snprintf(disassemblerCommand, sizeof(disassemblerCommand),
           "./disassembler %s", filename);

  // Capture the output of the disassembler
  char *disassemblerOutput = get_cout(disassemblerCommand);
  if (disassemblerOutput == NULL) {
    free(ref);
    errx(1, "Failed to execute command: %s\n", disassemblerCommand);
  }

  int match = strcmp(ref, disassemblerOutput);
  if (match == 0) {
    printOK(filename);
  } else {
    const char *errorLine = disassemblerOutput;
    while (*errorLine != '\0' && *errorLine == *ref) {
      ++errorLine;
      ++ref;
    }

    if (verbose) {
      printf("\nMissmatch for file %s:", filename);
      printDiff(errorLine, ref);
    } else {
      printKO(filename);
    }
  }
}

void cmp_files_dir(const char *directory, int verbose) {
  DIR *dir = opendir(directory);
  if (dir == NULL) {
    printf("Failed to open directory: %s\n", directory);
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) { // Check if it's a regular file
      char filepath[MAX_LENGTH];
      snprintf(filepath, sizeof(filepath), "%s%s", directory, entry->d_name);
      cmp_files(filepath, verbose);
    }
  }

  closedir(dir);
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3)
    errx(1, "Usage: %s [-v|--verbose] <file|directory>\n", argv[0]);

  if (argv[1][0] == '-' && strcmp(argv[1], "--verbose") &&
      strcmp(argv[1], "-v"))
    errx(1, "Invalid flag: %s\n", argv[1]);

  if (argc == 2 && argv[1][0] == '-')
    errx(1, "Provided flag without argument: %s\n", argv[1]);

  if (argc == 3 && strcmp(argv[1], "--verbose") && strcmp(argv[1], "-v"))
    errx(1, "Invalid flag: %s\n", argv[1]);

  // Check if the input is a directory
  DIR *dir = opendir(argv[argc - 1]);
  if (dir != NULL) {
    closedir(dir);
    cmp_files_dir(argv[argc - 1], argc == 3);
  } else {
    // Assume the input is a single file
    cmp_files(argv[argc - 1], argc == 3);
  }

  return 0;
}
