#include "main.h"

void print_headers(char *format, size_t byte_len, size_t line_len) {
  printf("          ");
  size_t i, j;
  for (i = 0; i < line_len; i++) {
    if (i % (line_len / 2) == 0 && i != 0)
      printf("  ");
    printf(format, i);
  }
  printf("\n");

  printf("--------");

  printf(" ");
  for (i = 0; i < line_len; i++) {
    if (i % (line_len / 2) == 0 && i != 0)
      printf("  ");
    printf(" ");
    for (j = 0; j < byte_len; j++)
      printf("-");
  }

  printf("\n");
}

void print_hexdump(const unsigned char *buffer, size_t buffer_size) {
  // print_headers(XFORMAT, 2, HEX_PER_LINE);

  size_t i, j;
  for (i = 0; i < buffer_size; i += HEX_PER_LINE) {
    printf("%08zx  ", i);

    for (j = 0; j < HEX_PER_LINE && (i + j) < buffer_size; j++) {
      if (j % (HEX_PER_LINE / 2) == 0 && j != 0)
        printf("  ");
      printf("%02x ", (unsigned char)buffer[i + j]);
    }

    if (j < HEX_PER_LINE) {
      size_t padding_size = (HEX_PER_LINE - j) * 3 + 2;
      printf("%*s", (int)padding_size, "");
    }

    printf(" |");
    for (j = 0; j < HEX_PER_LINE && (i + j) < buffer_size; j++) {
      unsigned char c = buffer[i + j];
      printf("%c", isprint(c) ? c : '.');
    }

    printf("|\n");
  }

  printf("%08zx\n\n", i - HEX_PER_LINE + j);
}

/// @param size in BYTES
void printb(const unsigned char *bin_str, size_t size, size_t byte_per_line) {
  // print_headers(BFORMAT, 8, byte_per_line);

  size_t i, j;
  for (i = 0; i < size; i += byte_per_line) {
    if (size > byte_per_line)
      printf("%08zx  ", i);

    for (j = 0; j < byte_per_line && (i + j) < size; j++) {
      if (j % (byte_per_line / 2) == 0 && j != 0)
        printf("  ");
      printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(bin_str[i + j]));
    }

    if (j < byte_per_line) {
      size_t padding_size = (byte_per_line - j) * 9 + 2;
      printf("%*s", (int)padding_size, "");
    }

    printf(" |");
    for (j = 0; j < byte_per_line && (i + j) < size; j++) {
      unsigned char c = bin_str[i + j];
      printf("%c", isprint(c) ? c : '.');
    }

    printf("|\n");
  }

  printf("%08zx\n\n", i - HEX_PER_LINE + j);
}

/// @param a address
/// @param il instruction length in bytes
/// @param ip instruction pointer
void printi(const unsigned char *p, size_t a, size_t il, size_t *ip) {
  printf("%04lx: ", a);
  for (size_t j = 0; j < il; j++) {
    printf("%02x", p[a + j]);
  }
  printf("%s", ASMFORMAT);

  *ip += il;
}

void hexdump(const char *path, unsigned char *buffer, size_t *buffer_size,
             size_t *program_size) {
  FILE *pfile;
  pfile = fopen(path, "rb");
  if (pfile == NULL)
    errx(1, "Provided file can't be opened!");

  size_t read_bytes = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, pfile);
  if (read_bytes == 0)
    errx(1, "Can't read the file!");

  fclose(pfile);

  *program_size = buffer[8];
  *buffer_size = read_bytes;
}

/// @brief Get addressing mode
/// @param a address after mod reg r/m byte
void get_adm(const unsigned char *p, size_t a, char *ea, size_t *disp,
             unsigned char mod, unsigned char rm) {
  if (mod == 0b00 && rm == 0b110) {
    *disp = p[a] + (p[a + 1] << 8);
    sprintf(ea, "[%04lx]", *disp);
    return;
  }

  switch (mod) {
  case 0b01:
    *disp = p[a];
    break;
  case 0b10:
    *disp = p[a] + (p[a + 1] << 8); // to recheck
    break;

  case 0b00:
  case 0b11:
    *disp = 0;
    break;

  default:
    errx(1, "Invalid mod value!");
  }

  char *dest;
  switch (rm) {
  case 0b000:
    dest = "bx+si";
    break;
  case 0b001:
    dest = "bx+di";
    break;
  case 0b010:
    dest = "bp+si";
    break;
  case 0b011:
    dest = "bp+di";
    break;
  case 0b100:
    dest = "si";
    break;
  case 0b101:
    dest = "di";
    break;
  case 0b110:
    dest = "bp";
    break;
  case 0b111:
    dest = "bx";
    break;

  default:
    errx(1, "Invalid rm value!");
  }

  // Recheck
  if (mod == 0b01 || mod == 0b10)
    sprintf(ea, "[%s+%zu]", dest, *disp);
  else if (mod == 0b00)
    sprintf(ea, "[%s]", dest);
  else
    sprintf(ea, "%s", dest);
}

void translate_bin(const unsigned char *p, size_t p_size) {
  const char *REG8[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
  const char *REG16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
  const char *SEG[4] = {"es", "cs", "ss", "ds"};

  unsigned char byte;
  unsigned char opcode, d, s, w, reg, mod, rm;
  unsigned char b7, b6, b5, b4, b3, b2, b1, b0;
  unsigned char p1, p2, p3, p4, p5, p6, p7, p8;

  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  size_t il, disp = 0, a = 0, ip = 0;

  while (a < p_size) {
    // Reset
    il = 0, disp = 0;
    d = s = w = reg = mod = rm = 0;
    b7 = b6 = b5 = b4 = b3 = b2 = b1 = b0 = 0;
    p1 = p2 = p3 = p4 = p5 = p6 = p7 = p8 = 0;

    byte = p[a];
    opcode = byte >> 4;

    switch (opcode) {
    case 0x0:
      b2 = (byte >> 2) & 0b1;

      if (!b2) {
        // ADD r/m and reg to either
        d = (byte >> 1) & 0b1;
        w = byte & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 1, ea, &disp, mod, rm);

        il = 2 + (disp ? 1 : 0);
        printi(p, a, il, &ip);

        if (d) {
          if (!w) {
            if (mod == 0b11) {
              printf("add %s, %s\n", REG8[reg], REG8[rm]);
            } else {
              printf("add %s, %s\n", REG8[reg], ea);
            }
          } else {
            if (mod == 0b11) {
              printf("add %s, %s\n", REG16[reg], REG16[rm]);
            } else {
              printf("add %s, %s\n", REG16[reg], ea);
            }
          }
        } else {
          if (!w) {
            if (mod == 0b11) {
              printf("add %s, %s\n", REG8[rm], REG8[reg]);
            } else {
              printf("add %s, %s\n", ea, REG8[reg]);
            }
          } else {
            if (mod == 0b11) {
              printf("add %s, %s\n", REG16[rm], REG16[reg]);
            } else {
              printf("add %s, %s\n", ea, REG16[reg]);
            }
          }
        }
      } else {
        printf("ADD: imm. to accumulator OR ???? (0x%02x)\n", byte);
      }

      break;

    case 0x3:
      b2 = (byte >> 2) & 0b1;
      w = byte & 0b1;

      if (!b2) {
        // XOR: r/m and reg to either
        il = 2;
        printi(p, a, il, &ip);

        d = (byte >> 1) & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, mod, rm);

        if (d) {
          if (!w) {
            if (mod == 0b11) {
              printf("xor %s, %s\n", REG8[reg], REG8[rm]);
            } else {
              printf("xor %s, %s\n", REG8[reg], ea);
            }
          } else {
            if (mod == 0b11) {
              printf("xor %s, %s\n", REG16[reg], REG16[rm]);
            } else {
              printf("xor %s, %s\n", REG16[reg], ea);
            }
          }
        } else {
          if (!w) {
            if (mod == 0b11) {
              printf("xor %s, %s\n", REG8[rm], REG8[reg]);
            } else {
              printf("xor %s, %s\n", REG8[rm], ea);
            }
          } else {
            if (mod == 0b11) {
              printf("xor %s, %s\n", REG16[rm], REG16[reg]);
            } else {
              printf("xor %s, %s\n", REG16[rm], ea);
            }
          }
        }
      } else {
        printf("XOR: imm. to accumulator OR ???? (0x%02x)\n", byte);
      }
      break;

    case 0x5:
      b3 = (byte >> 3) & 0b1;
      if (!b3) {
        // PUSH: reg
        reg = byte & 0b111;

        il = 1;
        printi(p, a, il, &ip);

        printf("push %s\n", REG16[reg]);
      } else {
        // POP: reg
        reg = byte & 0b111;

        il = 1;
        printi(p, a, il, &ip);

        printf("pop %s\n", REG16[reg]);
      }
      break;

    case 0x7:
      p4 = byte & 0b1111;
      switch (p4) {
      case 0x3:
        // JNB/JAE = Jump if not below/above or equal
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("jnb %04lx\n", ip + disp);
        break;

      case 0x5:
        // JNE/JNZ = Jump if not equal/zero
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("jne %04lx\n", ip + disp);
        break;

      default:
        printf("Unpatched case of JUMPs (0x%02x)\n", byte);
        break;
      }

      break;

    case 0x8:
      p4 = byte & 0b1111;
      if (p4 == 0xd) {
        // - LEA: reg <- EA
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, mod, rm);

        il = 2 + (disp ? 1 : 0);
        printi(p, a, il, &ip);

        printf("lea %s, %s\n", REG16[reg], ea);
      }

      p2 = (byte >> 2) & 0b11;
      switch (p2) {
      case 0b00:
        // CMP: immediate with r/m
        s = (byte >> 1) & 0b1;
        w = byte & 0b1;

        mod = (p[a + 1] >> 6) & 0b11;
        reg = 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, mod, rm); // offset data if mod != 11 ?

        il = 3 + ((!s && w) ? 1 : 0) + (disp ? 1 : 0); // to recheck
        printi(p, a, il, &ip);

        if (!s && w) {
          if (mod == 0b11) {
            printf("cmp %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          } else {
            printf("cmp %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
          }
        } else {
          if (mod == 0b11) {
            printf("cmp %s, %02x\n", REG8[rm], p[a + 2]);
          } else {
            printf("cmp %s, %02x\n", ea, p[a + 2]);
          }
        }
        break;

      case 0b01:
        b1 = (byte >> 1) & 0b1;
        if (!b1) {
          // TEST: immediate with r/m
          printf("TEST: r/m and reg (0x%02x)\n", byte);
        } else {
          // XCHG: r/m with reg
          printf("XCHG: r/m with reg (0x%02x)\n", byte);
        }
        break;

      case 0b10:
        // MOV: r/m <-> reg
        d = (byte >> 1) & 0b1;
        w = byte & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, mod, rm);

        il = (mod == 0b00 && rm == 0b110) ? 4 : 2 + !!disp;
        printi(p, a, il, &ip);

        if (d) {
          if (!w) {
            if (mod == 0b11)
              printf("mov %s, %s\n", REG8[reg], REG8[rm]);
            else
              printf("mov %s, %s\n", REG8[reg], ea);
          } else {
            if (mod == 0b11)
              printf("mov %s, %s\n", REG16[reg], REG16[rm]);
            else
              printf("mov %s, %s\n", REG16[reg], ea);
          }
        } else {
          if (!w) {
            if (mod == 0b11)
              printf("mov %s, %s\n", REG8[rm], REG8[reg]);
            else
              printf("mov %s, %s\n", ea, REG8[reg]);
          } else {
            if (mod == 0b11)
              printf("mov %s, %s\n", REG16[rm], REG16[reg]);
            else
              printf("mov %s, %s\n", ea, REG16[reg]);
          }
        }
        break;

      case 0b11:
        p2 = byte & 0b11;

        switch (p2) {
        case 0b10:
          // MOV: r/m -> sreg
          printf("MOV: r/m -> sreg (0x%02x)\n", byte);
          break;

        case 0b00:
          // MOV: sreg -> r/m
          printf("MOV: sreg -> r/m (0x%02x)\n", byte);
          break;
        }
        break;
      }
      break;

    case 0xb:
      // MOV: immediate to register
      w = (byte >> 3) & 0b1;
      reg = byte & 0b111;

      il = 2 + w; // might need to be changed
      printi(p, a, il, &ip);

      if (!w) {
        printf("mov %s, %02x\n", REG8[reg], p[a + 1]);
      } else {
        printf("mov %s, %04x\n", REG16[reg], p[a + 1]);
      }
      break;

    case 0xc: // INT: interrupt specified/unspecified type (deftype=3)
      b7 = byte & 0b1;
      il = 1 + b7;
      printi(p, a, il, &ip);

      if (!b7) {
        printf("int (DEFAULT=3, to catch if found)\n");
      } else {
        printf("int %02x\n", p[a + 1]);
      }
      break;

    case 0xe:
      // CALL: direct within segment
      il = 3;
      printi(p, a, il, &ip);
      printf("call %04lx\n", *(uint16_t *)&p[a + 1] + ip);
      break;

    case 0xf:
      p3 = (byte >> 1) & 0b111;
      if (p3 == 0b011) {
        // TEST: immediate to r/m
        w = byte & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = 0b000;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, mod, rm); // offset data if mod != 11 ?

        il = 3 + w; //+ (disp ? 1 : 0); // to recheck
        printi(p, a, il, &ip);

        if (!w) {
          if (mod == 0b11)
            printf("test %s, %x\n", REG8[rm], p[a + 2]);
          else
            printf("test %s, %x\n", ea, p[a + 2]);
        } else {
          if (mod == 0b11)
            printf("test %s, %x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          else
            printf("test %s, %x\n", ea, *(uint16_t *)&p[a + 2]);
        }
      } else {
        p4 = byte & 0b1111;
        switch (p4) {
        case 0x4:
          // HLT: halt
          il = 1;
          printi(p, a, il, &ip);
          printf("hlt\n");
          break;

        default:
          printf("Unmatched case with starting 0xf (0x%02x)\n", byte);
          break;
        }
      }
      break;

    default:
      printf("Unknown instruction (0x%02x)\n", byte);
      break;
    }

    // prevent infinite loop
    if (!il)
      il = 1;

    a += il;
  }
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3)
    errx(1, "Usage: %s <flags> <file>", argv[0]);

  if (argc == 2 && !strcmp(argv[1], "-dump"))
    errx(1, "Usage: %s -dump <file>", argv[0]);

  if (argc == 3 && argv[1][0] != '-')
    errx(1, "Provided flag not properly formatted: %s", argv[1]);

  if (argc == 3 && strcmp(argv[1], "-dump"))
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
    printb(program, program_size, BYTE_PER_LINE);
    printf("Text-area size: 0x%08lx = %zu bytes\n", program_size, program_size);
  }

  // Free & exit
  free(buffer);
  free(program);
  return 0;
}