#include "print_patterns.h"

const char *REG8[8];
const char *REG16[8];
const char *SEGREG[4];

void printsw(const unsigned char *p, size_t a, char *instr, size_t *ip,
             char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char rm = p[a + 1] & 0b111;
  get_adm(p, a + 2, mod, rm, ea, ds);

  switch (LSB4 & 0b11) {
  case 0b00:
    print4b(p, a, 2 + *ds + 1, ip);
    printf("%s %s%s, %x\n", instr, mod != 3 ? "byte " : "",
           mod == 3 ? REG8[rm] : ea, p[a + 2 + *ds]);
    break;

  case 0b01:
    print4b(p, a, 2 + *ds + 2, ip);
    printf("%s %s, %04x\n", instr, mod == 3 ? REG16[rm] : ea,
           *(uint16_t *)&p[a + 2 + *ds]);
    break;

  case 0b10:
    print4b(p, a, 2 + *ds, ip);
    printf("%s %s, %02x\n", instr, mod == 3 ? REG8[rm] : ea,
           *(uint8_t *)&p[a + 2]);
    break;

  case 0b11:
    print4b(p, a, 2 + *ds + 1, ip);
    int8_t imm8 = p[a + 2 + *ds];
    printf("%s %s, %s%x\n", instr, mod == 3 ? REG16[rm] : ea,
           imm8 < 0 ? "-" : "", imm8 < 0 ? -imm8 : imm8);
    break;
  }
}

void printvw(const unsigned char *p, size_t a, char *instr, size_t *ip,
             unsigned char LSB4, size_t *ds) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  unsigned char v = (LSB4 >> 1) & 0b1;
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char rm = p[a + 1] & 0b111;

  get_adm(p, a + 2, mod, rm, ea, ds);
  print4b(p, a, 2 + *ds, ip);
  if (!v) {
    if (!w) {
      if (mod == 0b11)
        printf("%s %s, 1\n", instr, REG8[rm]);
      else
        printf("%s %s, 1\n", instr, ea);
    } else {
      if (mod == 0b11)
        printf("%s %s, 1\n", instr, REG16[rm]);
      else
        printf("%s %s, 1\n", instr, ea);
    }
  } else {
    if (!w) {
      if (mod == 0b11)
        printf("%s %s, cl\n", instr, REG8[rm]);
      else
        printf("%s byte %s, cl\n", instr, ea);
    } else {
      if (mod == 0b11)
        printf("%s %s, cl\n", instr, REG16[rm]);
      else
        printf("%s %s, cl\n", instr, ea);
    }
  }

  free(ea);
}

void printdw(const unsigned char *p, size_t a, char *instr, size_t *ip,
             unsigned char LSB4, size_t *ds) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  unsigned char d = (LSB4 >> 1) & 0b1;
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char reg = (p[a + 1] >> 3) & 0b111;
  unsigned char rm = p[a + 1] & 0b111;

  get_adm(p, a + 2, mod, rm, ea, ds);
  print4b(p, a, 2 + *ds, ip);
  printf("%s %s, %s\n", instr,
         d ? (w ? REG16[reg] : REG8[reg])
           : (mod == 3 ? (w ? REG16[rm] : REG8[rm]) : ea),
         d ? (mod == 3 ? (w ? REG16[rm] : REG8[rm]) : ea)
           : (w ? REG16[reg] : REG8[reg]));

  free(ea);
}

void print_immtoacc(const unsigned char *p, size_t a, char *instr,
                    unsigned char w, size_t *ip) {
  print4b(p, a, 2 + w, ip);

  if (w) {
    printf("%s ax, %04x\n", instr, p[a + 1] + (p[a + 2] << 8));
  } else {
    printf("%s al, %x\n", instr, p[a + 1]);
  }
}

void print_immtorm(const unsigned char *p, size_t a, char *instr, size_t *ip,
                   char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char rm = p[a + 1] & 0b111;
  get_adm(p, a + 2, mod, rm, ea, ds);
  print4b(p, a, 3 + w + *ds, ip);

  if (w)
    printf("%s %s, %04x\n", instr, mod == 3 ? REG16[rm] : ea,
           *(uint16_t *)&p[a + 2]);
  else
    printf("%s %s, %02x\n", instr, mod == 3 ? REG8[rm] : ea,
           *(uint8_t *)&p[a + 2]);
}

void print_rmtoimm(const unsigned char *p, size_t a, char *instr, size_t *ip,
                   char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char rm = p[a + 1] & 0b111;
  get_adm(p, a + 2, mod, rm, ea, ds);
  print4b(p, a, 3 + w + *ds, ip);

  if (w) {
    if (mod == 0b11) {
      printf("%s %s, %04x\n", instr, REG16[rm], *(uint16_t *)&p[a + 2]);
    } else {
      printf("%s %s, %04x\n", instr, ea, *(uint16_t *)&p[a + 2 + *ds]);
    }
  } else {
    if (mod == 0b11) {
      printf("%s %s, %02x\n", instr, REG8[rm], *(uint8_t *)&p[a + 2]);
    } else {
      printf("%s %s, %02x\n", instr, ea, *(uint8_t *)&p[a + 2]);
    }
  }
}

void print_rm16(const unsigned char *p, size_t a, char *instr, unsigned char rm,
                size_t *ip) {
  print4b(p, a, 1, ip);
  printf("%s %s\n", instr, REG16[rm]);
}

void print_condjmps(const unsigned char *p, size_t a, char *instr, size_t *ip) {
  print4b(p, a, 2, ip);
  printf("%s %04lx\n", instr, *ip + (signed char)p[a + 1]);
}

void print_repmanip(const unsigned char *p, size_t a, char *instr, size_t *ip,
                    unsigned char w) {
  print4b(p, a, 2, ip);
  printf("rep %s%c\n", instr, w ? 'w' : 'b');
}

void print_single(const unsigned char *p, size_t a, char *instr, size_t *ip,
                  size_t il) {
  print4b(p, a, il, ip);
  printf("%s\n", instr);
}

void print_portfixed(const unsigned char *p, size_t a, char *instr, size_t *ip,
                     unsigned char w) {
  print4b(p, a, 2, ip);
  printf("%s %s, %x\n", instr, w ? "ax" : "al", p[a + 1]);
}

void print_portvar(const unsigned char *p, size_t a, char *instr, size_t *ip,
                   unsigned char w) {
  print4b(p, a, 1, ip);
  printf("%s %s, dx\n", instr, w ? "ax" : "al");
}

void print_loop(const unsigned char *p, size_t a, char *instr, size_t *ip) {
  print4b(p, a, 2, ip);
  printf("%s %04lx\n", instr, *ip + *(int8_t *)&p[a + 1]);
}

void print_calljmp(const unsigned char *p, size_t a, char *instr, size_t *ip) {
  print4b(p, a, 3, ip);
  printf("%s %04lx\n", instr, *ip + *(int16_t *)&p[a + 1]);
}

void print_stringmanip(const unsigned char *p, size_t a, char *instr,
                       unsigned char w, size_t *ip) {
  print4b(p, a, 1, ip);
  printf("%s%c\n", instr, w ? 'w' : 'b');
}

void print_fw(const unsigned char *p, size_t a, char *instr, size_t *ip,
              char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char rm = p[a + 1] & 0b111;
  print4b(p, a, 2 + *ds, ip);

  if (!w) {
    if (mod == 0b11)
      printf("%s %s\n", instr, REG8[rm]);
    else
      printf("%s %s\n", instr, ea);
  } else {
    if (mod == 0b11)
      printf("%s %s\n", instr, REG16[rm]);
    else
      printf("%s %s\n", instr, ea);
  }
}

void print_frm(const unsigned char *p, size_t a, char *instr, size_t *ip,
               char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (p[a + 1] >> 6) & 0b11;
  unsigned char rm = p[a + 1] & 0b111;

  get_adm(p, a + 2, mod, rm, ea, ds);
  print4b(p, a, 2 + *ds, ip);
  printf("%s %s\n", instr, mod == 3 ? REG16[rm] : ea);
}

void init_lookups() {
  REG8[0] = "al";
  REG8[1] = "cl";
  REG8[2] = "dl";
  REG8[3] = "bl";
  REG8[4] = "ah";
  REG8[5] = "ch";
  REG8[6] = "dh";
  REG8[7] = "bh";

  REG16[0] = "ax";
  REG16[1] = "cx";
  REG16[2] = "dx";
  REG16[3] = "bx";
  REG16[4] = "sp";
  REG16[5] = "bp";
  REG16[6] = "si";
  REG16[7] = "di";

  SEGREG[0] = "es";
  SEGREG[1] = "cs";
  SEGREG[2] = "ss";
  SEGREG[3] = "ds";
}