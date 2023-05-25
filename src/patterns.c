#include "patterns.h"

const char *REG8[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char *REG16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *SEGREG[4] = {"es", "cs", "ss", "ds"};

unsigned char d, w, s, v, z, mod, reg, rm;
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

    b2 = (LSB4 >> 2) & 0b1;
    if (!b2) {
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
      // OR: imm -> acc
      il = 2 + w;
      print4b(p, a, il, ip);

      if (!w) {
        printf("or al, %x\n", p[a + 1]);
      } else {
        printf("or ax, %04x\n", p[a + 1] + (p[a + 2] << 8));
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
      // ADD: imm -> acc
      w = LSB4 & 0b1;
      il = 2 + w;
      print4b(p, a, il, ip);

      if (!w) {
        printf("add al, %x\n", p[a + 1]);
      } else {
        printf("add ax, %04x\n", p[a + 1] + (p[a + 2] << 8));
      }
    }
  }

  free(ea);
}

void f0x1(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p2 = (LSB4 >> 2) & 0b11;
  switch (p2) {
  case 0b00:
    d = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);

    // ADC: r/m & reg <->
    if (d) {
      if (!w) {
        if (mod == 0b11) {
          printf("adc %s, %s\n", REG8[reg], REG8[rm]);
        } else {
          printf("adc %s, %s\n", REG8[reg], ea);
        }
      } else {
        if (mod == 0b11) {
          printf("adc %s, %s\n", REG16[reg], REG16[rm]);
        } else {
          printf("adc %s, %s\n", REG16[reg], ea);
        }
      }
    } else {
      if (!w) {
        if (mod == 0b11) {
          printf("adc %s, %s\n", REG8[rm], REG8[reg]);
        } else {
          printf("adc %s, %s\n", ea, REG8[reg]);
        }
      } else {
        if (mod == 0b11) {
          printf("adc %s, %s\n", REG16[rm], REG16[reg]);
        } else {
          printf("adc %s, %s\n", ea, REG16[reg]);
        }
      }
    }
    break;

  case 0b01:
    b1 = (LSB4 >> 1) & 0b1;
    if (!b1) {
      // ADC: imm -> acc
      w = LSB4 & 0b1;
      il = 2 + w;
      print4b(p, a, il, ip);

      if (!w) {
        printf("adc al, %02x\n", p[a + 1]);
      } else {
        printf("adc ax, %04x\n", p[a + 1] + (p[a + 2] << 8));
      }
    }
    break;

  case 0b10:
    d = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);

    // SBB: r/m & reg <->
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

  case 0b11:
    // SBB: imm <- acc
    w = LSB4 & 0b1;
    il = 2 + w;
    print4b(p, a, il, ip);

    if (!w) {
      printf("sbb al, %02x\n", p[a + 1]);
    } else {
      printf("sbb ax, %04x\n", p[a + 1] + (p[a + 2] << 8));
    }

    break;
  }

  free(ea);
}

void f0x2(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p4 = LSB4 & 0b1111;
  switch (p4) {
  case 0xf:
    // DAS
    print4b(p, a, 1, ip);
    printf("das\n");
    break;

  default:
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

    case 0b01:
      b1 = (LSB4 >> 1) & 0b1;
      if (!b1) {
        // AND: imm -> acc
        w = LSB4 & 0b1;
        il = 2 + w;
        print4b(p, a, il, ip);

        if (!w) {
          printf("and al, %02x\n", p[a + 1]);
        } else {
          printf("and ax, %04x\n", p[a + 1] + (p[a + 2] << 8));
        }
      }
      break;

    case 0b10:
      // SUB: r/m & reg <->
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
            printf("sub %s, %s\n", REG8[reg], REG8[rm]);
          } else {
            printf("sub %s, %s\n", REG8[reg], ea);
          }
        } else {
          if (mod == 0b11) {
            printf("sub %s, %s\n", REG16[reg], REG16[rm]);
          } else {
            printf("sub %s, %s\n", REG16[reg], ea);
          }
        }
      } else {
        if (!w) {
          if (mod == 0b11) {
            printf("sub %s, %s\n", REG8[rm], REG8[reg]);
          } else {
            printf("sub %s, %s\n", ea, REG8[reg]);
          }
        } else {
          if (mod == 0b11) {
            printf("sub %s, %s\n", REG16[rm], REG16[reg]);
          } else {
            printf("sub %s, %s\n", ea, REG16[reg]);
          }
        }
      }
      break;

    case 0b11:
      // SUB: imm. <- acc
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

    break;
  }

  free(ea);
}

void f0x3(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p4 = LSB4 & 0b1111;
  switch (p4) {
  case 0xf:
    // AAS
    print4b(p, a, 1, ip);
    printf("aas\n");
    break;

  default:
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
            printf("xor %s, %s\n", ea, REG8[reg]);
          }
        } else {
          if (mod == 0b11) {
            printf("xor %s, %s\n", REG16[rm], REG16[reg]);
          } else {
            printf("xor %s, %s\n", ea, REG16[reg]);
          }
        }
      }
      break;

    case 0b01:
      // XOR: imm -> acc
      w = LSB4 & 0b1;
      il = 2 + w;
      print4b(p, a, il, ip);

      if (!w) {
        printf("xor al, %02x\n", p[a + 1]);
      } else {
        printf("xor ax, %04x\n", *(uint16_t *)(p + a + 1));
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

    case 0b11:
      // CMP: imm. + acc
      w = LSB4 & 0b1;

      il = 2 + w;
      print4b(p, a, il, ip);

      if (!w) {
        printf("cmp al, %x\n", p[a + 1]);
      } else {
        printf("cmp ax, %04x\n", *(uint16_t *)(p + a + 1));
      }

      break;
    }
    break;
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

  print4b(p, a, 1, ip);
  printf("(undefined)\n");

  free(ea);
}

void f0x7(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p4 = LSB4 & 0b1111;

  il = 2;
  if (p4 != 0x9 && p4 != 0xa)
    print4b(p, a, il, ip);

  switch (p4) {
  case 0x0:
    // JO
    printf("jo");
    break;

  case 0x1:
    // JNO
    printf("jno");
    break;

  case 0x2:
    // JB/JNEA
    printf("jb");
    break;

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

  case 0x6:
    // JBE/JNA
    printf("jbe");
    break;

  case 0x7:
    // JNBE/JA
    printf("jnbe");
    break;

  case 0x8:
    // JS
    printf("js");
    break;

  case 0x9:
    // JNS
    print4b(p, a, 1, ip);
    printf("(undefined)\n");
    return;

  case 0xa:
    // JP/JPE
    print4b(p, a, 1, ip);
    printf("(undefined)\n");
    return;

  case 0xb:
    // JNP/JPO
    printf("jnp");
    break;

  case 0xc:
    // JL/JNGE
    printf("jl");
    break;

  case 0xd:
    // JNL/JGE
    printf("jnl");
    break;

  case 0xe:
    // JLE/JNG
    printf("jle");
    break;

  case 0xf:
    // JNLE/JG
    printf("jnle");
    break;

  default:
    printf("unmatched case of conditional jumps\n");
    break;
  }

  printf(SPACE "%04lx\n",
         *ip + (signed char)p[a + 1]); // assumed always a+1
  free(ea);
}

void f0x8(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p4 = LSB4 & 0b1111;
  switch (p4) {
  case 0xd:
    // LEA: reg <- EA
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);
    printf("lea %s, %s\n", REG16[reg], ea);
    break;

  case 0xf:
    // POP: r/m -- NOT MATCHED BY REF
    *ip += 2;
    printf("0x8f is not implemented\n");
    break;

  default:
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
            printf("add %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2 + ds]);
          } else {
            printf("add %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
          }
          break;

        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          int8_t imm8 = *(int8_t *)&p[a + 2 + ds];
          printf("add %s, %s%x\n", mod == 0b11 ? REG16[rm] : ea,
                 imm8 < 0 ? "-" : "", imm8 < 0 ? -imm8 : imm8);
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
            printf("or %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
          }
        } else {
          if (mod == 0b11) {
            printf("or %s, %02x\n", REG8[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("or %s, %02x\n", ea, *(uint8_t *)&p[a + 2]);
          }
        }
        break;

      case 0b010:
        // ADC: imm -> r/m
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        p2 = LSB4 & 0b11;
        switch (p2) { // sw
        case 0b01:
          il = 4 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("adc %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          } else {
            printf("adc %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
          }
          break;

        case 0b10:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("adc %s, %02x\n", REG8[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("adc %s, %02x\n", ea, *(uint8_t *)&p[a + 2]);
          }
          break;

        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("adc %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2 + ds]);
          } else {
            printf("adc %s, %x\n", ea, *(uint8_t *)&p[a + 2 + ds]);
          }

          break;

        default:
          printf("Unpatched case of ADC\n");
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
        case 0b01:
          il = 4 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("sbb %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          } else {
            printf("sbb %s, %04x\n", ea, *(uint16_t *)&p[a + 2]);
          }
          break;

        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("sbb %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2 + ds]);
          } else {
            printf("sbb %s, %x\n", ea, *(uint8_t *)&p[a + 2 + ds]);
          }
          break;

        default:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            printf("sbb %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("sbb byte %s, %x\n", ea, *(uint8_t *)&p[a + 2 + ds]);
          }
          break;
        }
        break;

      case 0b100:
        // AND: imm <- r/m
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 3 + w + ds;
        print4b(p, a, il, ip);

        if (w) {
          if (mod == 0b11) {
            printf("and %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          } else {
            printf("and %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
          }
        } else {
          if (mod == 0b11) {
            printf("and %s, %02x\n", REG8[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("and %s, %02x\n", ea, *(uint8_t *)&p[a + 2]);
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
            printf("sub %s, %x\n", ea, *(uint8_t *)&p[a + 2 + ds]);
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

      case 0b110:
        // XOR: imm -> r/m
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 3 + w + ds;
        print4b(p, a, il, ip);

        if (w) {
          if (mod == 0b11) {
            printf("xor %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
          } else {
            printf("xor %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
          }
        } else {
          if (mod == 0b11) {
            printf("xor %s, %02x\n", REG8[rm], *(uint8_t *)&p[a + 2]);
          } else {
            printf("xor %s, %02x\n", ea, *(uint8_t *)&p[a + 2]);
          }
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
            printf("cmp %s, %x\n", REG8[rm], *(uint8_t *)&p[a + 2]);
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
            printf("cmp %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
          }
          break;

        case 0b11:
          il = 3 + ds;
          print4b(p, a, il, ip);

          if (mod == 0b11) {
            // printf("HERE\n");
            int8_t tmp = *(int8_t *)&p[a + 2];

            if (tmp < 10) {
              if (tmp < 0) {
                tmp = -tmp;
                printf("cmp %s, -%d\n", REG16[rm], tmp); // sus
              } else {
                printf("cmp %s, %d\n", REG16[rm], tmp); // sus
              }
            } else {
              printf("cmp %s, %x\n", REG16[rm], *(uint8_t *)&p[a + 2]);
            }
          } else {

            int8_t imm8 = *(int8_t *)&p[a + 2 + ds];
            printf("cmp %s, %s%x\n", mod == 0b11 ? REG16[rm] : ea,
                   imm8 < 0 ? "-" : "", imm8 < 0 ? -imm8 : imm8);
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
        // TEST: r/m & reg <->
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
              printf("test %s, %s\n", REG8[reg], REG8[rm]);
            else
              printf("test %s, %s\n", REG8[reg], ea);
          } else {
            if (mod == 0b11)
              printf("test %s, %s\n", REG16[reg], REG16[rm]);
            else
              printf("test %s, %s\n", REG16[reg], ea);
          }
        } else {
          if (!w) {
            if (mod == 0b11)
              printf("test %s, %s\n", REG8[rm], REG8[reg]);
            else
              printf("test %s, %s\n", ea, REG8[reg]);
          } else {
            if (mod == 0b11)
              printf("test %s, %s\n", REG16[rm], REG16[reg]);
            else
              printf("test %s, %s\n", ea, REG16[reg]);
          }
        }
      } else {
        // XCHG: r/m + reg
        w = LSB4 & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (!w) {
          if (mod == 0b11)
            printf("xchg %s, %s\n", REG8[rm], REG8[reg]);
          else
            printf("xchg %s, %s\n", ea, REG8[reg]);
        } else {
          if (mod == 0b11)
            printf("xchg %s, %s\n", REG16[rm], REG16[reg]);
          else
            printf("xchg %s, %s\n", ea, REG16[reg]);
        }
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
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b11;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      il = 2 + ds;
      print4b(p, a, il, ip);

      p2 = LSB4 & 0b11;
      switch (p2) {
      case 0b00:
        // MOV: sreg -> r/m
        if (mod == 0b11)
          printf("mov %s, %s\n", REG16[rm], SEGREG[reg]);
        else
          printf("mov %s, %s\n", ea, SEGREG[reg]);
        break;

      case 0b10:
        // MOV: r/m -> sreg
        if (mod == 0b11)
          printf("mov %s, %s\n", SEGREG[reg], REG16[rm]);
        else
          printf("mov %s, %s\n", SEGREG[reg], ea);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}

void f0x9(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  b3 = (LSB4 >> 3) & 0b1;
  if (!b3) {
    // XCHG: reg + acc.
    reg = LSB4 & 0b111;
    il = 1;
    print4b(p, a, il, ip);
    printf("xchg %s, %s\n", REG16[reg], REG16[0]);
  } else {
    il = 1;
    print4b(p, a, il, ip);

    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0x8:
      // CBW
      printf("cbw\n");
      break;

    case 0x9:
      // CWD
      printf("cwd\n");
      break;

    case 0xb:
      // WAIT
      printf("wait\n");
      break;

    case 0xc:
      // PUSHF
      printf("pushf\n");
      break;

    case 0xd:
      // POPF
      printf("popf\n");
      break;

    case 0xe:
      // SAHF
      printf("sahf\n");
      break;

    case 0xf:
      // LAHF
      printf("lahf\n");
      break;

    default:
      printf("(undefined)\n");
      break;
    }
  }

  free(ea);
}

void f0xa(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  w = LSB4 & 0b1;
  p3 = (LSB4 >> 1) & 0b111;

  switch (p3) {
  case 0b000:
    // MOV: mem -> acc
    il = 3;
    print4b(p, a, il, ip);
    printf("mov %s, [%04x]\n", w ? "ax" : "al", *(uint16_t *)&p[a + 1]);
    break;

  case 0b001:
    // MOV: acc -> mem
    il = 3;
    print4b(p, a, il, ip);
    printf("mov [%04x], %s\n", *(uint16_t *)&p[a + 1], w ? "ax" : "al");
    break;

  case 0b010:
    // MOVS
    il = 1;
    print4b(p, a, il, ip);
    printf("movs%c\n", w ? 'w' : 'b');
    break;

  case 0b011:
    // CMPS
    il = 1;
    print4b(p, a, il, ip);
    printf("cmps%c\n", w ? 'w' : 'b');
    break;

  case 0b100:
    // TEST: imm & acc
    il = 2 + w;
    print4b(p, a, il, ip);

    if (!w) {
      printf("test al, %x\n", p[a + 1]);
    } else {
      printf("test ax, %x\n", *(uint16_t *)&p[a + 1]);
    }
    break;

  case 0b101:
    // STOS
    il = 1;
    print4b(p, a, il, ip);
    printf("stos%c\n", w ? 'w' : 'b');
    break;

  case 0b110:
    // LODS
    il = 1;
    print4b(p, a, il, ip);
    printf("lods%c\n", w ? 'w' : 'b');
    break;

  case 0b111:
    // SCAS
    il = 1;
    print4b(p, a, il, ip);
    printf("scas%c\n", w ? 'w' : 'b');
    break;

  default:
    printf("ERROR: unknown opcode (0x%02x)\n", LSB4);
    break;
  }

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
    printf("mov %s, %x\n", REG8[reg], p[a + 1]);
  } else {
    printf("mov %s, %04x\n", REG16[reg], *(uint16_t *)&p[a + 1]);
  }

  free(ea);
}

void f0xc(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p3 = (LSB4 >> 1) & 0b111;
  if (p3 == 0b011) {
    // MOV: imm -> r/m
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    if (w) {
      il = 4 + ds;
      print4b(p, a, il, ip);

      if (mod == 0b11) {
        printf("mov %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 3]);
      } else {
        printf("mov %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
      }
    } else {
      il = 3 + ds;
      print4b(p, a, il, ip);

      if (mod == 0b11) {
        printf("mov byte %s, %x\n", REG8[rm], p[a + 2 + ds]);
      } else {
        printf("mov byte %s, %x\n", ea, p[a + 2 + ds]);
      }
    }
  } else {
    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0x2:
      // RET: winseg + immed -> SP
      il = 3;
      print4b(p, a, il, ip);
      printf("ret %04x\n", *(uint16_t *)&p[a + 1]);
      break;

    case 0x3:
      // RET: winseg
      il = 1;
      print4b(p, a, il, ip);
      printf("ret\n");
      break;

    case 0x4:
      // LDS: ptr -> DS
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      il = 2 + ds;
      print4b(p, a, il, ip);
      printf("lds %s, %s\n", REG16[reg], ea);
      break;

    case 0x5:
      // LES: ptr -> ES
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      il = 2 + ds;
      print4b(p, a, il, ip);
      printf("les %s, %s\n", REG16[reg], ea);
      break;

    case 0xa:
      // RET: intersegment + immed -> SP
      il = 3;
      print4b(p, a, il, ip);
      printf("ret %04x\n", *(uint16_t *)&p[a + 1]);
      break;

    case 0xb:
      // RET: intersegement
      print4b(p, a, 1, ip);
      printf("ret (intersegment)\n");
      break;

    case 0xd:
      // INT: spec/unspec
      b7 = LSB4 & 0b1;
      il = 1 + b7;
      print4b(p, a, il, ip);

      if (!b7) {
        // INT: unspec, type = 3
        printf("int 3\n"); // not sure
      } else {
        printf("int %x\n", p[a + 1]);
      }
      break;

    case 0xe:
      // INTO
      print4b(p, a, 1, ip);
      printf("into\n");
      break;

    case 0xf:
      // IRET
      print4b(p, a, 1, ip);
      printf("iret\n");
      break;
    }
  }

  free(ea);
}

void f0xd(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  b3 = (LSB4 >> 3) & 0b1;
  if (b3) {
    // ESC
    mod = (p[a + 1] >> 6) & 0b11;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    il = 2 + ds;
    print4b(p, a, il, ip);
    printf("esc %s\n", ea);

    free(ea);
    return;
  }

  p4 = LSB4 & 0b1111;
  switch (p4) {
  case 0x4:
    // AAM
    if (p[a + 1] == 0x0a) {
      print4b(p, a, 2, ip);
      printf("aam\n");
    }
    break;

  case 0x5:
    // AAD
    if (p[a + 1] == 0x0a) {
      print4b(p, a, 2, ip);
      printf("aad\n");
    }
    break;

  case 0x7:
    // XLAT
    print4b(p, a, 1, ip);
    printf("xlat\n");
    break;

  default:
    v = (LSB4 >> 1) & 0b1;
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    switch (reg) {
    case 0b000:
      // ROL
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!v) {
        if (!w) {
          if (mod == 0b11)
            printf("rol %s, 1\n", REG8[rm]);
          else
            printf("rol %s, 1\n", ea);
        } else {
          if (mod == 0b11)
            printf("rol %s, 1\n", REG16[rm]);
          else
            printf("rol %s, 1\n", ea);
        }
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("rol %s, cl\n", REG8[rm]);
          else
            printf("rol %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("rol %s, cl\n", REG16[rm]);
          else
            printf("rol %s, cl\n", ea);
        }
      }
      break;

    case 0b001:
      // ROR
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!v) {
        if (!w) {
          if (mod == 0b11)
            printf("ror %s, 1\n", REG8[rm]);
          else
            printf("ror %s, 1\n", ea);
        } else {
          if (mod == 0b11)
            printf("ror %s, 1\n", REG16[rm]);
          else
            printf("ror %s, 1\n", ea);
        }
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("ror %s, cl\n", REG8[rm]);
          else
            printf("ror %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("ror %s, cl\n", REG16[rm]);
          else
            printf("ror %s, cl\n", ea);
        }
      }
      break;

    case 0b010:
      // RCL
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!v) {
        if (!w) {
          if (mod == 0b11)
            printf("rcl %s, 1\n", REG8[rm]);
          else
            printf("rcl %s, 1\n", ea);
        } else {
          if (mod == 0b11)
            printf("rcl %s, 1\n", REG16[rm]);
          else
            printf("rcl %s, 1\n", ea);
        }
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("rcl %s, cl\n", REG8[rm]);
          else
            printf("rcl %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("rcl %s, cl\n", REG16[rm]);
          else
            printf("rcl %s, cl\n", ea);
        }
      }
      break;

    case 0b011:
      // RCR
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!v) {
        if (!w) {
          if (mod == 0b11)
            printf("rcr %s, 1\n", REG8[rm]);
          else
            printf("rcr %s, 1\n", ea);
        } else {
          if (mod == 0b11)
            printf("rcr %s, 1\n", REG16[rm]);
          else
            printf("rcr %s, 1\n", ea);
        }
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("rcr %s, cl\n", REG8[rm]);
          else
            printf("rcr byte %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("rcr %s, cl\n", REG16[rm]);
          else
            printf("rcr %s, cl\n", ea);
        }
      }
      break;

    case 0b100:
      // SHL/SAL
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
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("shl %s, cl\n", REG8[rm]);
          else
            printf("shl %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("shl %s, cl\n", REG16[rm]);
          else
            printf("shl %s, cl\n", ea);
        }
      }
      break;

    case 0b101: // patch v flag!!!
      // SHR
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!v) {
        if (!w) {
          if (mod == 0b11)
            printf("shr %s, 1\n", REG8[rm]);
          else
            printf("shr %s, 1\n", ea);
        } else {
          if (mod == 0b11)
            printf("shr %s, 1\n", REG16[rm]);
          else
            printf("shr %s, 1\n", ea);
        }
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("shr %s, cl\n", REG8[rm]);
          else
            printf("shr %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("shr %s, cl\n", REG16[rm]);
          else
            printf("shr %s, cl\n", ea);
        }
      }
      break;

    case 0b111: // patch v flag!!!
      // SAR
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!v) {
        if (!w) {
          if (mod == 0b11)
            printf("sar %s, 1\n", REG8[rm]);
          else
            printf("sar %s, 1\n", ea);
        } else {
          if (mod == 0b11)
            printf("sar %s, 1\n", REG16[rm]);
          else
            printf("sar %s, 1\n", ea);
        }
      } else {
        if (!w) {
          if (mod == 0b11)
            printf("sar %s, cl\n", REG8[rm]);
          else
            printf("sar %s, cl\n", ea);
        } else {
          if (mod == 0b11)
            printf("sar %s, cl\n", REG16[rm]);
          else
            printf("sar %s, cl\n", ea);
        }
      }
      break;

    default:
      il = 1;
      print4b(p, a, il, ip);
      printf("(undefined)\n"); // d9
      break;
    }
    break;
  }

  free(ea);
}

void f0xe(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p3 = (LSB4 >> 1) & 0b111;
  switch (p3) {
  case 0b010:
    // IN: fixed port
    w = LSB4 & 0b1;
    il = 2;
    print4b(p, a, il, ip);
    printf("in %s, %x\n", w ? "ax" : "al", p[a + 1]);
    break;

  case 0b011:
    // OUT: fixed port
    w = LSB4 & 0b1;
    il = 2;
    print4b(p, a, il, ip);
    printf("out %s, %x\n", w ? "ax" : "al", p[a + 1]);
    break;

  case 0b110:
    // IN: variable port
    w = LSB4 & 0b1;
    il = 1;
    print4b(p, a, il, ip);
    printf("in %s, dx\n", w ? "ax" : "al");
    break;

  case 0b111:
    // OUT: variable port
    w = LSB4 & 0b1;
    il = 1;
    print4b(p, a, il, ip);
    printf("out %s, dx\n", w ? "ax" : "al");
    break;

  default:
    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0x0:
      // LOOPNZ/LOOPNE
      il = 2;
      print4b(p, a, il, ip);
      printf("loopnz %04lx\n", *ip + *(int8_t *)&p[a + 1]);
      break;

    case 0x1:
      // LOOPZ/LOOPE
      il = 2;
      print4b(p, a, il, ip);
      printf("loopz %04lx\n", *ip + *(int8_t *)&p[a + 1]);
      break;

    case 0x2:
      // LOOP
      il = 2;
      print4b(p, a, il, ip);
      printf("loop %04lx\n", *ip + *(int8_t *)&p[a + 1]);
      break;

    case 0x3:
      // JCXZ
      il = 2;
      print4b(p, a, il, ip);
      printf("jcxz %04lx\n", *ip + *(int8_t *)&p[a + 1]);
      break;

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
    break;
  }

  free(ea);
}

void f0xf(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  p3 = (LSB4 >> 1) & 0b111;
  switch (p3) {
  case 0b001:
    // REP
    z = LSB4 & 0b1;

    il = 2;
    print4b(p, a, il, ip);

    printf("rep "); // since z = 1 ???

    p4 = p[a + 1] & 0b1111;
    w = p4 & 0b1;
    switch (p4 >> 1) {
    case 0b010:
      printf("movs");
      break;

    case 0b011:
      printf("cmps");
      break;

    case 0b101:
      printf("stos");
      break;

    case 0b110:
      printf("lods");
      break;

    case 0b111:
      printf("scas");
      break;

    default:
      printf("Unmatched case of 0xf and rep: %02x\n", LSB4);
      break;
    }

    printf("%c\n", w ? 'w' : 'b');
    break;

  case 0b011:
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
          printf("test byte %s, %x\n", ea, p[a + 2 + ds]); // to recheck byte
      } else {
        if (mod == 0b11)
          printf("test %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 2]);
        else
          printf("test %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
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

    case 0b101:
      // IMUL
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!w) {
        if (mod == 0b11)
          printf("imul %s\n", REG8[rm]);
        else
          printf("imul %s\n", ea);
      } else {
        if (mod == 0b11)
          printf("imul %s\n", REG16[rm]);
        else
          printf("imul %s\n", ea);
      }
      break;

    case 0b110:
      // DIV
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!w) {
        if (mod == 0b11)
          printf("div %s\n", REG8[rm]);
        else
          printf("div %s\n", ea);
      } else {
        if (mod == 0b11)
          printf("div %s\n", REG16[rm]);
        else
          printf("div %s\n", ea);
      }
      break;

    case 0b111:
      // IDIV
      il = 2 + ds;
      print4b(p, a, il, ip);

      if (!w) {
        if (mod == 0b11)
          printf("idiv %s\n", REG8[rm]);
        else
          printf("idiv %s\n", ea);
      } else {
        if (mod == 0b11)
          printf("idiv %s\n", REG16[rm]);
        else
          printf("idiv %s\n", ea);
      }
      break;

    default:
      printf("Unmatched case of 0xf + 0b011 as p3: %02x\n", LSB4);
      break;
    }
    break;

  default:
    p4 = LSB4 & 0b1111;
    switch (p4) {
    case 0x0:
      // LOCK
      print4b(p, a, 1, ip);
      printf("lock\n");
      break;

    case 0x4:
      // HLT
      print4b(p, a, 1, ip);
      printf("hlt\n");
      break;

    case 0x5:
      // CMC
      print4b(p, a, 1, ip);
      printf("cmc\n");
      break;

    case 0x8:
      // CLC
      print4b(p, a, 1, ip);
      printf("clc\n");
      break;

    case 0x9:
      // STC
      print4b(p, a, 1, ip);
      printf("stc\n");
      break;

    case 0xa:
      // CLI
      print4b(p, a, 1, ip);
      printf("cli\n");
      break;

    case 0xb:
      // STI
      print4b(p, a, 1, ip);
      printf("sti\n");
      break;

    case 0xc:
      // CLD
      print4b(p, a, 1, ip);
      printf("cld\n");
      break;

    case 0xd:
      // STD
      print4b(p, a, 1, ip);
      printf("std\n");
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

      case 0b001:
        // DEC: r/m
        w = LSB4 & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("dec %s\n", REG16[rm]);
        else
          printf("dec %s\n", ea); // not sure, to recheck
        break;

      case 0b010:
        // CALL: indw|seg
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("call %s\n", REG16[rm]);
        else
          printf("call %s\n", ea);
        break;

      case 0b011:
        // CALL: indirect intersegment
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("call %s\n", REG16[rm]);
        else
          printf("call %s\n", ea);
        break;

      case 0b100:
        // JMP: indw|seg
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("jmp %s\n", REG16[rm]);
        else
          printf("jmp %s\n", ea); // not sure, to recheck
        break;

      case 0b101:
        // JMP: indirect intersegment
        mod = (p[a + 1] >> 6) & 0b11;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);

        il = 2 + ds;
        print4b(p, a, il, ip);

        if (mod == 0b11)
          printf("jmp %s\n", REG16[rm]);
        else
          printf("jmp %s\n", ea);
        break;

      case 0b110:
        // PUSH: r/m
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
        printf("Unmatched case 0xf (0x%02x + reg matching)\n", LSB4);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}