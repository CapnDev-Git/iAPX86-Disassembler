#include "patterns.h"

const char *REG8[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char *REG16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *SEGREG[4] = {"es", "cs", "ss", "ds"};

unsigned char d, w, s, v, mod, reg, rm;
unsigned char b7, b6, b5, b4, b3, b2, b1, b0;
unsigned char p1, p2, p3, p4, p5, p6, p7, p8;
size_t il = 0, ds = 0, disp = 0;
char *ea;

void f0x0(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  b3 = (LSB4 >> 3) & 0b1;
  if (b3) {
    // OR: r/m & reg <->
    d = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);

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
    b2 = (LSB4 >> 2) & 0b1;
    if (!b2) {
      // ADD: r/m & reg <->
      d = (LSB4 >> 1) & 0b1;
      w = LSB4 & 0b1;
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      il = 2 + ds;
      print4b(p, a, il, ip);

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
      printf("ADD: imm. to accumulator OR ???? (0x%02x)\n", LSB4);
    }
  }

  free(ea);
}

void f0x1(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  // SBB: r/m & reg <->
  d = (LSB4 >> 1) & 0b1;
  w = LSB4 & 0b1;
  mod = (p[a + 1] >> 6) & 0b11;
  reg = (p[a + 1] >> 3) & 0b111;
  rm = p[a + 1] & 0b111;
  get_adm(p, a + 2, mod, rm, ea, &ds);

  il = 2 + ds;
  print4b(p, a, il, ip);

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

  free(ea);
}

void f0x2(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p2 = (LSB4 >> 2) & 0b11;

  switch (p2) {
  case 0b00:
    // AND: r/m & reg <->
    d = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);

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

  case 0b11:
    // SUB: imm. -> acc
    w = LSB4 & 0b1;

    il = 2 + w;
    print4b(p, a, il, ip);

    if (!w) {
      printf("sub al, %02x\n", p[a + 1]);
    } else {
      printf("sub ax, %04x\n", *(uint16_t *)(p + a + 1));
    }
    break;
  }

  free(ea);
}

void f0x3(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p2 = (LSB4 >> 2) & 0b11;
  switch (p2) {
  case 0b00:
    // XOR: r/m & reg <->
    d = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2;
    print4b(p, a, il, ip);

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
    break;

  case 0b10:
    // CMP: r/m & reg
    d = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);

    if (d) {
      if (!w) {
        if (mod == 0b11) {
          printf("cmp %s, %s\n", REG8[reg], REG8[rm]);
        } else {
          printf("cmp %s, %s\n", REG8[reg], ea);
        }
      } else {
        if (mod == 0b11) {
          printf("cmp %s, %s\n", REG16[reg], REG16[rm]);
        } else {
          printf("cmp %s, %s\n", REG16[reg], ea);
        }
      }
    } else {
      if (!w) {
        if (mod == 0b11) {
          printf("cmp %s, %s\n", REG8[rm], REG8[reg]);
        } else {
          printf("cmp %s, %s\n", ea, REG8[reg]);
        }
      } else {
        if (mod == 0b11) {
          printf("cmp %s, %s\n", REG16[rm], REG16[reg]);
        } else {
          printf("cmp %s, %s\n", ea, REG16[reg]);
        }
      }
    }

    break;
  }

  if (!b2) {

  } else {
    printf("XOR: imm. to accumulator OR ???? (0x%02x)\n", LSB4);
  }

  free(ea);
}

void f0x4(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  b3 = (LSB4 >> 3) & 0b1;
  if (b3) {
    // DEC: r/m
    reg = LSB4 & 0b111;

    il = 1;
    print4b(p, a, il, ip);
    printf("dec %s\n", REG16[reg]);
  } else {
    // INC: r/m
    reg = LSB4 & 0b111;

    il = 1;
    print4b(p, a, il, ip);
    printf("inc %s\n", REG16[reg]);
  }

  free(ea);
}

void f0x5(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  b3 = (LSB4 >> 3) & 0b1;
  reg = LSB4 & 0b111;
  if (!b3) {
    // PUSH: reg
    il = 1;
    print4b(p, a, il, ip);
    printf("push %s\n", REG16[reg]);
  } else {
    // POP: reg
    il = 1;
    print4b(p, a, il, ip);
    printf("pop %s\n", REG16[reg]);
  }

  free(ea);
}

void f0x6(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  *ip++; // TODO, remove this
  printf("UNMATCHED: 0x6%01x\n", LSB4);

  free(ea);
}

void f0x7(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  il = 2;
  print4b(p, a, il, ip);

  p4 = LSB4 & 0b1111;
  switch (p4) {
  case 0x3:
    // JNB/JAE
    printf("jnb");
    break;

  case 0x4:
    // JE/JZ
    printf("je");
    break;

  case 0x5:
    // JNE/JNZ
    printf("jne");
    break;

  case 0xc:
    // JL/JNGE
    printf("jl");
    break;

  case 0xd:
    // JNL/JGE
    printf("jnl");
    break;

  case 0xf:
    // JNLE/JG
    printf("jnle");
    break;

  default:
    printf("Unpatched case of JUMPs (0x%02x)\n", LSB4);
    break;
  }

  printf(SPACE "%04lx\n",
         *ip + (unsigned char)p[a + 1]); // assumed always a+1
  free(ea);
}

void f0x8(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p4 = LSB4 & 0b1111;
  if (p4 == 0xd) {
    // - LEA: reg <- EA
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);

    printf("lea %s, %s\n", REG16[reg], ea);
  } else {
    p2 = (LSB4 >> 2) & 0b11;
    switch (p2) {
    case 0b00:
      s = (LSB4 >> 1) & 0b1;
      w = LSB4 & 0b1;
      reg = (p[a + 1] >> 3) & 0b111;

      switch (reg) {
      case 0b000:
        // ADD: imm + r/m
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        p2 = LSB4 & 0b11;
        switch (p2) { // sw
        case 0b01:
          il = 4 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("add %s, %x\n", REG16[rm], *(uint16_t *)&p[a + 2]); //+3 ?
          } else {
            printf("add %s, %x\n", ea, *(uint16_t *)&p[a + 2]); //+3 ?
          }
          break;

        default: // sw = 0b00, 0b10, 0b11
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) { // why REG16 here, to recheck
            printf("add %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("add %s, %x\n", ea, *(uint8_t *)&p[a + 2]);
          }
          break;
        }
        break;

      case 0b011:
        // SBB: imm <- r/m
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        p2 = LSB4 & 0b11;
        switch (p2) { // sw
        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("sbb %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 3]);
          } else {
            printf("sbb %s, %x\n", ea, *(uint8_t *)&p[a + 3]);
          }
          break;

        default:
          printf("Unpatched case of SBB (0x%02x)\n", LSB4);
          break;
        }
        break;

      case 0b001:
        // OR: imm <- r/m
        // only need w here, no s
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 3 + w + ds;
        print4b(p, a, il, ip);

        if (w) {
          if (mod == 0b11) {
            printf("or %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          } else {
            printf("or %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
          }
        } else {
          if (mod == 0b11) {
            printf("or %s, %02x\n", REG8[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("or %s, %02x\n", ea, *(uint8_t *)&p[a + 2]);
          }
        }
        break;

      case 0b101:
        // SUB: imm <- r/m
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        p2 = LSB4 & 0b11;
        switch (p2) { // sw
        case 0b01:
          il = 4 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("sub %s, %04x\n", REG16[rm],
                   *(uint16_t *)&p[a + 2]); // to recheck
          } else {
            printf("sub %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
          }
          break;

        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("sub %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("sub %s, %x\n", ea, *(uint8_t *)&p[a + 2]);
          }
          break;

        default: // to recheck
          il = 2 + ds;
          print4b(p, a, il, ip);

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
        get_adm(p, a + 2, mod, rm, ea, &ds);

        p2 = LSB4 & 0b11;
        switch (p2) { // sw
        case 0b00:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("cmp byte %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("cmp byte %s, %x\n", ea, *(uint8_t *)&p[a + 2 + ds]);
          }
          break;

        case 0b01:
          il = 4 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("cmp %s, %04x\n", REG16[rm],
                   *(uint16_t *)&p[a + 2]); // to recheck
          } else {
            printf("cmp %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
          }
          break;

        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            // printf("HERE\n");
            printf("cmp %s, %d\n", REG16[rm], (*(int8_t *)&p[a + 2])); // sus
          } else {
            printf("cmp %s, %x\n", ea, *(uint8_t *)&p[a + 2 + ds]);
          }
          break;

        default: // to recheck
          il = 2 + ds;
          print4b(p, a, il, ip);

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
      b1 = (LSB4 >> 1) & 0b1;
      if (!b1) {
        // TEST: imm + r/m
        printf("TEST: r/m and reg (0x%02x)\n", LSB4);
      } else {
        // XCHG: r/m + reg
        printf("XCHG: r/m with reg (0x%02x)\n", LSB4);
      }
      break;

    case 0b10:
      // MOV: r/m <-> reg
      d = (LSB4 >> 1) & 0b1;
      w = LSB4 & 0b1;
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      // recheck size
      il = 2 + ds;
      print4b(p, a, il, ip);

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
      p2 = LSB4 & 0b11;

      switch (p2) {
      case 0b10:
        // MOV: r/m -> sreg
        printf("MOV: r/m -> sreg (0x%02x)\n", LSB4);
        break;

      case 0b00:
        // MOV: sreg -> r/m
        printf("MOV: sreg -> r/m (0x%02x)\n", LSB4);
        break;
      }
      break;
    }
  }

  free(ea);
}

void f0x9(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p4 = LSB4 & 0b1111;
  switch (p4) {
  case 0x8:
    // CBW
    il = 1;
    print4b(p, a, il, ip);
    printf("cbw\n");
    break;

  case 0x9:
    // CWD
    il = 1;
    print4b(p, a, il, ip);
    printf("cwd\n");
    break;
  }

  free(ea);
}

void f0xa(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  *ip++; // TODO, remove this
  printf("UNMATCHED: 0xa%01x\n", LSB4);

  free(ea);
}

void f0xb(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  // MOV: imm -> register
  w = (LSB4 >> 3) & 0b1;
  reg = LSB4 & 0b111;

  il = 2 + w; // might need to be changed
  print4b(p, a, il, ip);

  if (!w) {
    printf("mov %s, %02x\n", REG8[reg], p[a + 1]);
  } else {
    printf("mov %s, %04x\n", REG16[reg], *(uint16_t *)&p[a + 1]);
  }

  free(ea);
}

void f0xc(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p3 = (LSB4 >> 1) & 0b111;
  if (p3 == 0b011) {
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    switch (reg) {
    case 0b000:
      // MOV: imm. -> r/m
      if (w) {
        il = 4 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11) {
          printf("mov %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 3]);
        } else {
          printf("mov %s, %04x\n", ea, *(uint16_t *)&p[a + 3]); // +3 weird
        }
      } else {
        il = 3 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11) {
          printf("mov byte %s, %02x\n", REG8[rm], p[a + 2]); // byte weird
        } else {
          printf("mov byte %s, %02x\n", ea, p[a + 2]);
        }
      }

      break;
    }
  } else {

    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0b0011:
      // RET: winseg
      il = 1;
      print4b(p, a, il, ip);
      printf("ret\n");

      break;

    case 0b1101:
      // INT: spec/unspec
      b7 = LSB4 & 0b1;
      il = 1 + b7;
      print4b(p, a, il, ip);

      if (!b7) {
        printf("int (DEFAULT=3, to catch if found)\n");
      } else {
        printf("int %02x\n", p[a + 1]);
      }
      break;
    }
  }

  free(ea);
}

void f0xd(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  // might change to case per case
  v = (LSB4 >> 1) & 0b1;
  w = LSB4 & 0b1;
  mod = (p[a + 1] >> 6) & 0b11;
  reg = (p[a + 1] >> 3) & 0b111;
  rm = p[a + 1] & 0b111;
  get_adm(p, a + 2, mod, rm, ea, &ds);

  switch (reg) { // patch v flag!!!
  case 0b100:
    // SHL/SAL: r/m
    il = 2 + ds;
    print4b(p, a, il, ip);

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
    printf("Unmatched case 0xd (0x%02x)\n", LSB4);
    break;
  }

  free(ea);
}

void f0xe(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p3 = (LSB4 >> 1) & 0b111;
  if (p3 == 0b010) {
    // IN: fixed port
    w = LSB4 & 0b1;
    il = 2;
    print4b(p, a, il, ip);
    printf("in %s, %02x\n", w ? "ax" : "al", p[a + 1]);
  } else if (p3 == 0b110) {
    // IN: variable port
    w = LSB4 & 0b1;
    il = 1;
    print4b(p, a, il, ip);
    printf("in %s, dx\n", w ? "ax" : "al");
  } else {
    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0x8:
      // CALL: dirseg
      il = 3;
      print4b(p, a, il, ip);
      printf("call %04lx\n", *ip + *(int16_t *)&p[a + 1]);
      break;

    case 0x9:
      // JMP: dirseg
      il = 3;
      print4b(p, a, il, ip);
      printf("jmp %04lx\n", *ip + *(int16_t *)&p[a + 1]);
      break;

    case 0xb:
      // JMP SHORT: dirseg short
      il = 2;
      print4b(p, a, il, ip);
      printf("jmp short %04lx\n", *ip + *(int8_t *)&p[a + 1]);
      break;

    default:
      printf("Unmatched case of 0xe: %02x\n", LSB4);
      break;
    }
  }

  free(ea);
}

void f0xf(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p3 = (LSB4 >> 1) & 0b111;
  if (p3 == 0b011) {
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    switch (reg) {
    case 0b000:
      // TEST: imm -> r/m
      il = 3 + w + ds; //+ (disp ? 1 : 0); // to recheck
      print4b(p, a, il, ip);

      if (!w) {
        if (mod == 0b11)
          printf("test %s, %x\n", REG8[rm], p[a + 2]);
        else
          printf("test byte %s, %x\n", ea, p[a + 3]); // to recheck byte
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
      print4b(p, a, il, ip);

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
      print4b(p, a, il, ip);

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

    case 0b100:
      // MUL
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!w) {
        if (mod == 0b11)
          printf("mul %s\n", REG8[rm]);
        else
          printf("mul %s\n", ea);
      } else {
        if (mod == 0b11)
          printf("mul %s\n", REG16[rm]);
        else
          printf("mul %s\n", ea);
      }
      break;

    default:
      printf("Unmatched case of 0xf + 0b011 as p3: %02x\n", LSB4);
      break;
    }
  } else {
    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0x4:
      // HLT
      il = 1;
      print4b(p, a, il, ip);
      printf("hlt\n");
      break;

    case 0xf:
      reg = (p[a + 1] >> 3) & 0b111;
      switch (reg) {

      case 0b000:
        // INC: r/m
        w = LSB4 & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("inc %s\n", REG16[rm]);
        else
          printf("inc %s\n", ea); // not sure, to recheck
        break;

      case 0b010:
        // CALL: indseg
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("call %s\n", REG16[rm]);
        else
          printf("call %s\n", ea); // not sure, to recheck
        break;

      case 0b110:
        // PUSH: reg
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("push %s\n", REG16[rm]);
        else
          printf("push %s\n", ea); // not sure, to recheck
        break;

      default:
        printf("Unmatched case 0xf (0x%02x)\n", LSB4);
        break;
      }
      break;
    }
  }

  free(ea);
}
