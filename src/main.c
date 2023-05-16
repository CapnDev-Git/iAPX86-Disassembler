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
  }
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
    errx(1, "Provided file is empty!");

  fclose(pfile);

  memcpy(program_size, buffer + 8, 4);
  *buffer_size = read_bytes;
}

/// @brief Get addressing mode
/// @param a address after mod reg r/m byte
void get_adm(const unsigned char *p, size_t a, char *ea, size_t *disp,
             size_t *ds, unsigned char mod, unsigned char rm) {
  if (mod == 0b00 && rm == 0b110) {
    *disp = p[a] + (p[a + 1] << 8); // len = 2 bytes
    *ds = 2;
    sprintf(ea, "[%04lx]", *disp);
    return;
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
  }

  switch (mod) {
  case 0b00:
    *disp = 0;
    *ds = 0;
    sprintf(ea, "[%s]", dest);
    break;

  case 0b01:
    *disp = p[a];
    *ds = 1;

    unsigned char abs_disp = (*disp & 0x80) ? (~*disp + 1) : *disp;
    sprintf(ea, "[%s%c%x]", dest, (*disp & 0x80) ? '-' : '+', abs_disp);
    break;

  case 0b10:
    *disp = p[a] + (p[a + 1] << 8);
    *ds = 2;
    sprintf(ea, "[%s+%lx]", dest, *disp);

    break;

  case 0b11:
    *disp = 0;
    *ds = 0;
    sprintf(ea, "%s", dest);
    break;
  }
}

void translate_bin(const unsigned char *p, size_t p_size) {
  const char *REG8[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
  const char *REG16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
  const char *SEG[4] = {"es", "cs", "ss", "ds"};

  unsigned char byte;
  unsigned char opcode, d, s, v, w, reg, mod, rm;
  unsigned char b7, b6, b5, b4, b3, b2, b1, b0;
  unsigned char p1, p2, p3, p4, p5, p6, p7, p8;

  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  size_t il, disp = 0, ds = 0, a = 0, ip = 0;

  while (a < p_size) {
    // Reset
    il = 0, disp = ds = 0;
    d = s = v = w = reg = mod = rm = 0;
    b7 = b6 = b5 = b4 = b3 = b2 = b1 = b0 = 0;
    p1 = p2 = p3 = p4 = p5 = p6 = p7 = p8 = 0;

    byte = p[a];
    opcode = byte >> 4;

    switch (opcode) {
    case 0x0:
      if (a + 1 >= p_size) {
        printi(p, a, 1, &ip);
        printf("(undefined)\n");
        return; // to recheck
      }

      b3 = (byte >> 3) & 0b1;
      if (b3) {
        // OR: r/m & reg <->
        d = (byte >> 1) & 0b1;
        w = byte & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

        il = 2 + ds;
        printi(p, a, il, &ip);

        if (d) {
          if (!w) {
            if (mod == 0b11) {
              printf("or %s, %s\n", REG8[reg], REG8[rm]);
            } else {
              printf("or %s, %s\n", REG8[reg], ea);
            }
          } else {
            if (mod == 0b11) {
              printf("or %s, %s\n", REG16[reg], REG16[rm]);
            } else {
              printf("or %s, %s\n", REG16[reg], ea);
            }
          }
        } else {
          if (!w) {
            if (mod == 0b11) {
              printf("or %s, %s\n", REG8[rm], REG8[reg]);
            } else {
              printf("or %s, %s\n", ea, REG8[reg]);
            }
          } else {
            if (mod == 0b11) {
              printf("or %s, %s\n", REG16[rm], REG16[reg]);
            } else {
              printf("or %s, %s\n", ea, REG16[reg]);
            }
          }
        }
      } else {
        b2 = (byte >> 2) & 0b1;
        if (!b2) {
          // ADD: r/m & reg <->
          d = (byte >> 1) & 0b1;
          w = byte & 0b1;
          mod = (p[a + 1] >> 6) & 0b11;
          reg = (p[a + 1] >> 3) & 0b111;
          rm = p[a + 1] & 0b111;
          get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

          il = 2 + ds;
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
      }

      break;

    case 0x1:
      // SBB: r/m & reg <->

      d = (byte >> 1) & 0b1;
      w = byte & 0b1;
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

      il = 2 + ds;
      printi(p, a, il, &ip);

      if (d) {
        if (!w) {
          if (mod == 0b11) {
            printf("sbb %s, %s\n", REG8[reg], REG8[rm]);
          } else {
            printf("sbb %s, %s\n", REG8[reg], ea);
          }
        } else {
          if (mod == 0b11) {
            printf("sbb %s, %s\n", REG16[reg], REG16[rm]);
          } else {
            printf("sbb %s, %s\n", REG16[reg], ea);
          }
        }
      } else {
        if (!w) {
          if (mod == 0b11) {
            printf("sbb %s, %s\n", REG8[rm], REG8[reg]);
          } else {
            printf("sbb %s, %s\n", ea, REG8[reg]);
          }
        } else {
          if (mod == 0b11) {
            printf("sbb %s, %s\n", REG16[rm], REG16[reg]);
          } else {
            printf("sbb %s, %s\n", ea, REG16[reg]);
          }
        }
      }
      break;

    case 0x2:
      // AND: r/m & reg <->
      d = (byte >> 1) & 0b1;
      w = byte & 0b1;
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 1, ea, &disp, &ds, mod, rm);

      il = 2 + ds;
      printi(p, a, il, &ip);

      if (d) {
        if (!w) {
          if (mod == 0b11) {
            printf("and %s, %s\n", REG8[reg], REG8[rm]);
          } else {
            printf("and %s, %s\n", REG8[reg], ea);
          }
        } else {
          if (mod == 0b11) {
            printf("and %s, %s\n", REG16[reg], REG16[rm]);
          } else {
            printf("and %s, %s\n", REG16[reg], ea);
          }
        }
      } else {
        if (!w) {
          if (mod == 0b11) {
            printf("and %s, %s\n", REG8[rm], REG8[reg]);
          } else {
            printf("and %s, %s\n", ea, REG8[reg]);
          }
        } else {
          if (mod == 0b11) {
            printf("and %s, %s\n", REG16[rm], REG16[reg]);
          } else {
            printf("and %s, %s\n", ea, REG16[reg]);
          }
        }
      }

      break;

    case 0x3:
      b2 = (byte >> 2) & 0b1;
      w = byte & 0b1;

      if (!b2) {
        // XOR: r/m & reg <->
        il = 2;
        printi(p, a, il, &ip);

        d = (byte >> 1) & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

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

    case 0x4:
      b3 = (byte >> 3) & 0b1;

      if (b3) {
        // DEC: r/m
        reg = byte & 0b111;

        il = 1;
        printi(p, a, il, &ip);
        printf("dec %s\n", REG16[reg]);
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
        // JNB/JAE
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("jnb %04lx\n", ip + disp);
        break;

      case 0x4:
        // JE/JZ
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("je %04lx\n", ip + disp);
        break;

      case 0x5:
        // JNE/JNZ
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("jne %04lx\n", ip + disp);
        break;

      case 0xc:
        // JL/JNGE
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("jl %04lx\n", ip + disp);
        break;

      case 0xd:
        // JNL/JGE
        il = 2;
        printi(p, a, il, &ip);

        disp = p[a + 1];
        printf("jnl %04lx\n", ip + disp);
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
        get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

        il = 2 + ds;
        printi(p, a, il, &ip);

        printf("lea %s, %s\n", REG16[reg], ea);
      } else {
        p2 = (byte >> 2) & 0b11;
        switch (p2) {
        case 0b00:
          s = (byte >> 1) & 0b1;
          w = byte & 0b1;
          reg = (p[a + 1] >> 3) & 0b111;

          switch (reg) {
          case 0b000:
            // ADD: imm + r/m
            mod = (p[a + 1] >> 6) & 0b11;
            rm = p[a + 1] & 0b111;
            get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

            p2 = byte & 0b11;
            switch (p2) { // sw
            case 0b01:
              il = 4 + ds;
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("add %s, %x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
              } else {
                printf("add %s, %x\n", ea, *(uint16_t *)&p[a + 2]);
              }
              break;

            default: // sw = 0b00, 0b10, 0b11
              il = 3 + ds;
              printi(p, a, il, &ip);

              if (mod == 0b11) { // why REG16 here, to recheck
                printf("add %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
              } else {
                printf("add %s, %x\n", ea, *(uint8_t *)&p[a + 2]);
              }
              break;
            }
            break;

          case 0b101:
            // SUB: imm <- r/m
            mod = (p[a + 1] >> 6) & 0b11;
            rm = p[a + 1] & 0b111;
            get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

            p2 = byte & 0b11;
            switch (p2) { // sw
            case 0b01:
              il = 4 + ds; // to recheck if mod != 11
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("sub %s, %x\n", REG16[rm],
                       *(uint16_t *)&p[a + 2]); // to recheck
              } else {
                printf("sub %s, %x\n", ea, *(uint16_t *)&p[a + 2]);
              }
              break;

            case 0b11:
              il = 3 + ds; // to recheck if mod != 11
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("sub %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
              } else {
                printf("sub %s, %x\n", ea, *(uint8_t *)&p[a + 2]);
              }
              break;

            default:       // to recheck
              il = 2 + ds; // to recheck if mod != 11
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("sub %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
              } else {
                printf("sub %s, %x\n", ea, *(uint8_t *)&p[a + 2]);
              }
              break;
            }
            break;

          case 0b111:
            // CMP: imm + r/m
            mod = (p[a + 1] >> 6) & 0b11;
            rm = p[a + 1] & 0b111;
            get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

            p2 = byte & 0b11;
            switch (p2) { // sw
            case 0b01:
              il = 4 + ds; // to recheck if mod != 11
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("cmp %s, %04x\n", REG16[rm],
                       *(uint16_t *)&p[a + 2]); // to recheck
              } else {
                printf("cmp %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
              }
              break;

            case 0b11:
              il = 3 + ds; // to recheck if mod != 11
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("cmp %s, %x\n", REG16[rm], *(uint8_t *)&p[a + ds + 1]);
              } else {
                printf("cmp %s, %x\n", ea, *(uint8_t *)&p[a + ds + 2]);
              }
              break;

            default:       // to recheck
              il = 2 + ds; // to recheck if mod != 11
              printi(p, a, il, &ip);

              if (mod == 0b11) {
                printf("cmp %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
              } else {
                printf("cmp %s, %x\n", ea, *(uint8_t *)&p[a + 2]);
              }
              break;
            }
          }
          break;

        case 0b01:
          b1 = (byte >> 1) & 0b1;
          if (!b1) {
            // TEST: imm + r/m
            printf("TEST: r/m and reg (0x%02x)\n", byte);
          } else {
            // XCHG: r/m + reg
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
          get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

          // recheck size
          il = 2 + ds;
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
      }
      break;

    case 0xb:
      // MOV: imm -> register
      w = (byte >> 3) & 0b1;
      reg = byte & 0b111;

      il = 2 + w; // might need to be changed
      printi(p, a, il, &ip);

      if (!w) {
        printf("mov %s, %02x\n", REG8[reg], p[a + 1]);
      } else {
        printf("mov %s, %04x\n", REG16[reg], *(uint16_t *)&p[a + 1]);
      }
      break;

    case 0xc:
      p4 = byte & 0b1111;
      switch (p4) {
      case 0b0011:
        // RET: winseg
        il = 1;
        printi(p, a, il, &ip);
        printf("ret\n");

        break;

      case 0b1101:
        // INT: spec/unspec
        b7 = byte & 0b1;
        il = 1 + b7;
        printi(p, a, il, &ip);

        if (!b7) {
          printf("int (DEFAULT=3, to catch if found)\n");
        } else {
          printf("int %02x\n", p[a + 1]);
        }
        break;
      }
      break;

    case 0xd:
      // might change to case per case
      v = (byte >> 1) & 0b1;
      w = byte & 0b1;
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

      switch (reg) { // patch v flag!!!
      case 0b100:
        // SHL/SAL: r/m
        il = 2 + ds;
        printi(p, a, il, &ip);

        if (!v) {
          if (!w) {
            if (mod == 0b11)
              printf("shl %s, 1\n", REG8[rm]);
            else
              printf("shl %s, 1\n", ea);
          } else {
            if (mod == 0b11)
              printf("shl %s, 1\n", REG16[rm]);
            else
              printf("shl %s, 1\n", ea);
          }
        } // else case to determine (setup CL register in some way...)
        break;

      default:
        printf("Unmatched case 0xd (0x%02x)\n", byte);
        break;
      }
      break;

    case 0xe:
      p3 = (byte >> 1) & 0b111;
      if (p3 == 0b010) {
        // IN: fixed port
        w = byte & 0b1;
        il = 2;
        printi(p, a, il, &ip);
        printf("in %s, %02x\n", w ? "ax" : "al", p[a + 1]);
      } else if (p3 == 0b110) {
        // IN: variable port
        w = byte & 0b1;
        il = 1;
        printi(p, a, il, &ip);
        printf("in %s, dx\n", w ? "ax" : "al");
      } else {
        p4 = byte & 0b1111;
        switch (p4) {
        case 0x8:
          // CALL: dirseg
          il = 3;
          printi(p, a, il, &ip);
          printf("call %04lx\n", *(int16_t *)&p[a + 1] + ip);
          break;

        case 0x9:
          // JMP: dirseg
          il = 3;
          printi(p, a, il, &ip);
          printf("jmp %04lx\n", *(int16_t *)&p[a + 1] + ip);
          break;

        case 0xb:
          // JMP SHORT: dirseg short
          il = 2;
          printi(p, a, il, &ip);
          printf("jmp short %04lx\n", *(int8_t *)&p[a + 1] + ip);
          break;

        default:
          printf("Unmatched case of 0xe: %02x\n", byte);
          break;
        }
      }
      break;

    case 0xf:
      p3 = (byte >> 1) & 0b111;
      if (p3 == 0b011) {
        w = byte & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

        switch (reg) {
        case 0b000:
          // TEST: imm -> r/m
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
          break;

        case 0b010:
          // NOT
          il = 2 + ds;
          printi(p, a, il, &ip);

          if (!w) {
            if (mod == 0b11)
              printf("not %s\n", REG8[rm]);
            else
              printf("not %s\n", ea);
          } else {
            if (mod == 0b11)
              printf("not %s\n", REG16[rm]);
            else
              printf("not %s\n", ea);
          }
          break;

        case 0b011:
          // NOT
          il = 2 + ds;
          printi(p, a, il, &ip);

          if (!w) {
            if (mod == 0b11)
              printf("neg %s\n", REG8[rm]);
            else
              printf("neg %s\n", ea);
          } else {
            if (mod == 0b11)
              printf("neg %s\n", REG16[rm]);
            else
              printf("neg %s\n", ea);
          }
          break;
        }
      } else {
        p4 = byte & 0b1111;
        switch (p4) {
        case 0x4:
          // HLT
          il = 1;
          printi(p, a, il, &ip);
          printf("hlt\n");
          break;

        case 0xf:
          reg = (p[a + 1] >> 3) & 0b111;
          switch (reg) {
          case 0b010:
            // CALL: indseg
            mod = (p[a + 1] >> 6) & 0b11;
            rm = p[a + 1] & 0b111;
            get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

            il = 2 + ds;
            printi(p, a, il, &ip);

            if (mod == 0b11)
              printf("call %s\n", REG16[rm]);
            else
              printf("call %s\n", ea); // not sure, to recheck
            break;

          case 0b110:
            // PUSH: reg
            mod = (p[a + 1] >> 6) & 0b11;
            rm = p[a + 1] & 0b111;
            get_adm(p, a + 2, ea, &disp, &ds, mod, rm);

            il = 2 + ds;
            printi(p, a, il, &ip);

            if (mod == 0b11)
              printf("push %s\n", REG16[rm]);
            else
              printf("push %s\n", ea); // not sure, to recheck
            break;

          default:
            printf("Unmatched case 0xf (0x%02x)\n", byte);
            break;
          }
          break;
        }
      }
      break;

    default:
      printf("Unmatched case (0x%02x)\n", byte);
      break;
    }

    if (!il)
      il = 1; // prevent infinite loop
    a += il;
  }
}

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