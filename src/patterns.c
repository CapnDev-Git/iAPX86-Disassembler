#include "patterns.h"

unsigned char d, w, s, v, z;
unsigned char mod, reg, rm;
size_t ds = 0;

// TODO recheck guards for accumulator & <->

void f0x0(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  if (((LSB4 >> 3) & 0b1)) {
    if ((LSB4 >> 2) & 0b1) {
      // OR: imm -> acc
      print_immtoacc(p, a, "or", LSB4 & 0b1, ip);
    } else {
      // OR: r/m & reg <->
      printdw(p, a, "or", ip, LSB4, &ds);
    }
  } else { // TODO recheck, add guard ?
    if ((LSB4 >> 2) & 0b1) {
      // ADD: imm -> acc
      print_immtoacc(p, a, "add", LSB4 & 0b1, ip);
    } else {
      // ADD: r/m & reg <->
      printdw(p, a, "add", ip, LSB4, &ds);
    }
  }
}

void f0x1(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  switch ((LSB4 >> 2) & 0b11) {
  case 0b00:
    // ADC: r/m & reg <->
    printdw(p, a, "adc", ip, LSB4, &ds);
    break;

  case 0b01:
    if (!((LSB4 >> 1) & 0b1)) { // TODO recheck, guard ?
      // ADC: imm -> acc
      print_immtoacc(p, a, "adc", LSB4 & 0b1, ip);
    }
    break;

  case 0b10:
    // SBB: r/m & reg <->
    printdw(p, a, "sbb", ip, LSB4, &ds);
    break;

  case 0b11:
    // SBB: imm <- acc
    print_immtoacc(p, a, "sbb", LSB4 & 0b1, ip);
    break;
  }
}

void f0x2(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  switch (LSB4 & 0b1111) {
  case 0xf:
    // DAS
    print_single(p, a, "das", ip, 1);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // AND: r/m & reg <->
      printdw(p, a, "and", ip, LSB4, &ds);
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) { // TODO recheck, guard ?
        // AND: imm -> acc
        print_immtoacc(p, a, "and", LSB4 & 0b1, ip);
      }
      break;

    case 0b10:
      // SUB: r/m & reg <->
      printdw(p, a, "sub", ip, LSB4, &ds);
      break;

    case 0b11: // TODO fix this case ?
      // SUB: imm. <- acc
      w = LSB4 & 0b1;
      print4b(p, a, 2 + w, ip);

      if (!w) {
        printf("sub al, %02x\n", p[a + 1]);
      } else {
        printf("sub ax, %04x\n", *(uint16_t *)(p + a + 1));
      }

      // print_immtoacc(p, a, "sub", LSB4 & 0b1, ip);
      break;
    }

    break;
  }
}

void f0x3(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  switch (LSB4 & 0b1111) {
  case 0xf:
    // AAS
    print_single(p, a, "aas", ip, 1);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // XOR: r/m & reg <->
      printdw(p, a, "xor", ip, LSB4, &ds);
      break;

    case 0b01:
      // XOR: imm -> acc
      print_immtoacc(p, a, "xor", LSB4 & 0b1, ip);
      break;

    case 0b10:
      // CMP: r/m & reg
      printdw(p, a, "cmp", ip, LSB4, &ds);
      break;

    case 0b11:
      // CMP: imm. + acc
      print_immtoacc(p, a, "cmp", LSB4 & 0b1, ip);
      break;
    }
    break;
  }
}

void f0x4(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    // DEC: r/m
    print_rm16(p, a, "dec", LSB4 & 0b111, ip);
  } else {
    // INC: r/m
    print_rm16(p, a, "inc", LSB4 & 0b111, ip);
  }

  free(ea);
}

void f0x5(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    // POP: reg
    print_rm16(p, a, "pop", LSB4 & 0b111, ip);
  } else {
    // PUSH: reg
    print_rm16(p, a, "push", LSB4 & 0b111, ip);
  }

  free(ea);
}

void f0x6(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  print_single(p, a, "(undefined)", ip, 1);
  free(ea);
}

void f0x7(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch (LSB4 & 0b1111) {
  case 0x0:
    // JO
    print_condjmps(p, a, "jo", ip);
    break;

  case 0x1:
    // JNO
    print_condjmps(p, a, "jno", ip);
    break;

  case 0x2:
    // JB/JNEA
    print_condjmps(p, a, "jb", ip);
    break;

  case 0x3:
    // JNB/JAE
    print_condjmps(p, a, "jnb", ip);
    break;

  case 0x4:
    // JE/JZ
    print_condjmps(p, a, "je", ip);
    break;

  case 0x5:
    // JNE/JNZ
    print_condjmps(p, a, "jne", ip);
    break;

  case 0x6:
    // JBE/JNA
    print_condjmps(p, a, "jbe", ip);
    break;

  case 0x7:
    // JNBE/JA
    print_condjmps(p, a, "jnbe", ip);
    break;

  case 0x8:
    // JS
    print_condjmps(p, a, "js", ip);
    break;

  case 0x9:
    // JNS
    // print_condjmps(p, a, "jns", ip);
    print_single(p, a, "(undefined)", ip, 1);

    break;

  case 0xa:
    // JP/JPE
    // print_condjmps(p, a, "jp", ip);
    print_single(p, a, "(undefined)", ip, 1);
    break;

  case 0xb:
    // JNP/JPO
    print_condjmps(p, a, "jnp", ip);
    break;

  case 0xc:
    // JL/JNGE
    print_condjmps(p, a, "jl", ip);
    break;

  case 0xd:
    // JNL/JGE
    print_condjmps(p, a, "jnl", ip);
    break;

  case 0xe:
    // JLE/JNG
    print_condjmps(p, a, "jle", ip);
    break;

  case 0xf:
    // JNLE/JG
    print_condjmps(p, a, "jnle", ip);
    break;
  }

  free(ea);
}

void f0x8(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch (LSB4 & 0b1111) {
  case 0xd: // TODO fix this case
    // LEA: reg <- EA
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    print4b(p, a, 2 + ds, ip);
    printf("lea %s, %s\n", REG16[reg], ea);
    break;

  case 0xf:
    // POP: r/m
    *ip += 2;
    printf("0x8f is not implemented\n"); // TODO fix
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      switch ((p[a + 1] >> 3) & 0b111) {
      case 0b000:
        // ADD: imm -> r/m
        printsw(p, a, "add", ip, ea, LSB4, &ds);
        break;

      case 0b001: // EXCEPTION, NO SW HERE
        // OR: imm <- r/m
        print_rmtoimm(p, a, "or", ip, ea, LSB4, &ds);
        break;

      case 0b010:
        // ADC: imm -> r/m
        printsw(p, a, "adc", ip, ea, LSB4, &ds);
        break;

      case 0b011:
        // SBB: imm <- r/m
        printsw(p, a, "sbb", ip, ea, LSB4, &ds);
        break;

      case 0b100: // NO SW
        // AND: imm <- r/m
        print_rmtoimm(p, a, "and", ip, ea, LSB4, &ds);
        break;

      case 0b101:
        // SUB: imm <- r/m
        printsw(p, a, "sub", ip, ea, LSB4, &ds);
        break;

      case 0b110: // SW not needed
        // XOR: imm -> r/m
        print_immtorm(p, a, "xor", ip, ea, LSB4, &ds);
        break;

      case 0b111:
        // CMP: imm + r/m
        printsw(p, a, "cmp", ip, ea, LSB4, &ds);
        break;
      }
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        // TEST: r/m & reg <->
        printdw(p, a, "test", ip, LSB4, &ds);
      } else {
        // XCHG: r/m + reg
        w = LSB4 & 0b1;
        mod = (p[a + 1] >> 6) & 0b11;
        reg = (p[a + 1] >> 3) & 0b111;
        rm = p[a + 1] & 0b111;
        get_adm(p, a + 2, mod, rm, ea, &ds);
        print4b(p, a, 2 + ds, ip);

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
      printdw(p, a, "mov", ip, LSB4, &ds);
      break;

    case 0b11:
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b11;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);
      print4b(p, a, 2 + ds, ip);

      switch (LSB4 & 0b11) {
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

  if ((LSB4 >> 3) & 0b1) {
    switch (LSB4 & 0b1111) {
    case 0x8:
      // CBW
      print_single(p, a, "cbw", ip, 1);
      break;

    case 0x9:
      // CWD
      print_single(p, a, "cwd", ip, 1);
      break;

    case 0xb:
      // WAIT
      // print_single(p, a, "wait", ip, 1);
      print_single(p, a, "(undefined)", ip, 1);
      break;

    case 0xc:
      // PUSHF
      print_single(p, a, "pushf", ip, 1);
      break;

    case 0xd:
      // POPF
      print_single(p, a, "popf", ip, 1);
      break;

    case 0xe:
      // SAHF
      print_single(p, a, "sahf", ip, 1);
      break;

    case 0xf:
      // LAHF
      print_single(p, a, "lahf", ip, 1);
      break;

    default:
      print_single(p, a, "(undefined)", ip, 1);
      break;
    }
  } else { // TODO patch this print
    // XCHG: reg + acc.
    print4b(p, a, 1, ip);
    printf("xchg %s, ax\n", REG16[LSB4 & 0b111]);
  }

  free(ea);
}

void f0xa(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  w = LSB4 & 0b1;
  switch ((LSB4 >> 1) & 0b111) {
  case 0b000: // TODO: special case and next case also
    // MOV: mem -> acc
    print4b(p, a, 3, ip);
    printf("mov %s, [%04x]\n", w ? "ax" : "al", *(uint16_t *)&p[a + 1]);
    break;

  case 0b001:
    // MOV: acc -> mem
    print4b(p, a, 3, ip);
    printf("mov [%04x], %s\n", *(uint16_t *)&p[a + 1], w ? "ax" : "al");
    break;

  case 0b010:
    // MOVS
    print_stringmanip(p, a, "movs", LSB4 & 0b1, ip);
    break;

  case 0b011:
    // CMPS
    print_stringmanip(p, a, "cmps", LSB4 & 0b1, ip);
    break;

  case 0b100:
    // TEST: imm & acc
    print_immtoacc(p, a, "test", LSB4 & 0b1, ip);
    break;

  case 0b101:
    // STOS
    print_stringmanip(p, a, "stos", LSB4 & 0b1, ip);
    break;

  case 0b110:
    // LODS
    print_stringmanip(p, a, "lods", LSB4 & 0b1, ip);
    break;

  case 0b111:
    // SCAS
    print_stringmanip(p, a, "scas", LSB4 & 0b1, ip);
    break;
  }

  free(ea);
}

void f0xb(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  // MOV: imm -> register
  w = (LSB4 >> 3) & 0b1;
  reg = LSB4 & 0b111;

  print4b(p, a, 2 + w, ip);

  if (!w) {
    printf("mov %s, %x\n", REG8[reg], p[a + 1]);
  } else {
    printf("mov %s, %04x\n", REG16[reg], *(uint16_t *)&p[a + 1]);
  }

  free(ea);
}

void f0xc(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b011: // TODO: fix this case
    // MOV: imm -> r/m
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    reg = (p[a + 1] >> 3) & 0b111;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    if (w) {
      print4b(p, a, 4 + ds, ip);

      if (mod == 0b11) {
        printf("mov %s, %04x\n", REG16[rm], *(uint16_t *)&p[a + 3]);
      } else {
        printf("mov %s, %04x\n", ea, *(uint16_t *)&p[a + 2 + ds]);
      }
    } else {
      print4b(p, a, 3 + ds, ip);

      if (mod == 0b11) {
        printf("mov byte %s, %x\n", REG8[rm], p[a + 2 + ds]);
      } else {
        printf("mov byte %s, %x\n", ea, p[a + 2 + ds]);
      }
    }
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x2: // TODO: clean this case
      // RET: winseg + immed -> SP
      print4b(p, a, 3, ip);
      printf("ret %04x\n", *(uint16_t *)&p[a + 1]);

      break;

    case 0x3:
      // RET: winseg
      print_single(p, a, "ret", ip, 1);
      break;

    case 0x4: // TODO: clean this case
      // LDS: ptr -> DS
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      print4b(p, a, 2 + ds, ip);
      printf("lds %s, %s\n", REG16[reg], ea);
      break;

    case 0x5: // TODO: clean this case
      // LES: ptr -> ES
      mod = (p[a + 1] >> 6) & 0b11;
      reg = (p[a + 1] >> 3) & 0b111;
      rm = p[a + 1] & 0b111;
      get_adm(p, a + 2, mod, rm, ea, &ds);

      print4b(p, a, 2 + ds, ip);
      printf("les %s, %s\n", REG16[reg], ea);
      break;

    case 0xa: // TODO: clean this case
      // RET: intersegment + immed -> SP
      // print4b(p, a, 3, ip);
      // printf("ret %04x\n", *(uint16_t *)&p[a + 1]);
      print_single(p, a, "(undefined)", ip, 1);

      break;

    case 0xb:
      // RET: intersegement
      print_single(p, a, "ret (intersegment)", ip, 1);
      break;

    case 0xd:
      if (LSB4 & 0b1) {
        // INT: spec
        print4b(p, a, 2, ip);
        printf("int %x\n", p[a + 1]);
      } else {
        // INT: unspec, type = 3
        print_single(p, a, "int (3)", ip, 1);
      }
      break;

    case 0xe:
      // INTO
      print_single(p, a, "into", ip, 1);
      break;

    case 0xf:
      // IRET
      print_single(p, a, "iret", ip, 1);
      break;

    default:
      print_single(p, a, "(undefined)", ip, 1);
      break;
    }
    break;
  }

  free(ea);
}

void f0xd(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  // ESC not matched
  switch (LSB4 & 0b1111) {
  case 0x4:
    // AAM
    if (p[a + 1] == 0x0a)
      print_single(p, a, "aam", ip, 2);
    break;

  case 0x5:
    // AAD
    if (p[a + 1] == 0x0a)
      print_single(p, a, "aad", ip, 2);
    break;

  case 0x7:
    // XLAT
    print_single(p, a, "xlat", ip, 1);
    break;

  default:
    switch ((p[a + 1] >> 3) & 0b111) {
    case 0b000:
      // ROL
      // printvw(p, a, "rol", ip, LSB4);
      print_single(p, a, "(undefined)", ip, 1);

      break;

    case 0b001:
      // ROR
      printvw(p, a, "ror", ip, LSB4, &ds);
      break;

    case 0b010:
      // RCL
      printvw(p, a, "rcl", ip, LSB4, &ds);
      break;

    case 0b011:
      // RCR
      printvw(p, a, "rcr", ip, LSB4, &ds);
      break;

    case 0b100:
      // SHL/SAL
      printvw(p, a, "shl", ip, LSB4, &ds);
      break;

    case 0b101:
      // SHR
      printvw(p, a, "shr", ip, LSB4, &ds);
      break;

    case 0b111:
      // SAR
      printvw(p, a, "sar", ip, LSB4, &ds);
      break;

    default:
      print_single(p, a, "(undefined)", ip, 1);
      break;
    }
    break;
  }
}

void f0xe(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b010:
    // IN: fixed port
    print_portfixed(p, a, "in", ip, LSB4 & 0b1);
    break;

  case 0b011:
    // OUT: fixed port
    print_portfixed(p, a, "out", ip, LSB4 & 0b1);
    break;

  case 0b110:
    // IN: variable port
    print_portvar(p, a, "in", ip, LSB4 & 0b1);
    break;

  case 0b111:
    // OUT: variable port
    print_portvar(p, a, "out", ip, LSB4 & 0b1);
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOOPNZ/LOOPNE
      print_loop(p, a, "loopnz", ip);
      break;

    case 0x1:
      // LOOPZ/LOOPE
      print_loop(p, a, "loopz", ip);
      break;

    case 0x2:
      // LOOP
      print_loop(p, a, "loop", ip);
      break;

    case 0x3:
      // JCXZ
      print_loop(p, a, "jcxz", ip);
      break;

    case 0x8:
      // CALL: dirseg
      print_calljmp(p, a, "call", ip);
      break;

    case 0x9:
      // JMP: dirseg
      print_calljmp(p, a, "jmp", ip);
      break;

    case 0xb:
      // JMP SHORT: dirseg short
      print_loop(p, a, "jmp short", ip);
      break;

    default:
      print_single(p, a, "(undefined)", ip, 1);
      break;
    }
    break;
  }

  free(ea);
}

void f0xf(const unsigned char *p, size_t a, unsigned char LSB4, size_t *ip) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b001:
    // REP (z flag)
    z = LSB4 & 0b1;
    unsigned char p4 = p[a + 1] & 0b1111;
    switch (p4 >> 1) {
    case 0b010:
      // REP + MOVS
      print_repmanip(p, a, "movs", ip, p4 & 0b1);
      break;

    case 0b011:
      // REP + CMPS
      print_repmanip(p, a, "cmps", ip, p4 & 0b1);
      break;

    case 0b101:
      // REP + STOS
      print_repmanip(p, a, "stos", ip, p4 & 0b1);
      break;

    case 0b110:
      // REP + LODS
      print_repmanip(p, a, "lods", ip, p4 & 0b1);
      break;

    case 0b111:
      // REP + SCAS
      print_repmanip(p, a, "scas", ip, p4 & 0b1);
      break;

    default:
      print_single(p, a, "(undefined)", ip, 1);
      break;
    }
    break;

  case 0b011:
    w = LSB4 & 0b1;
    mod = (p[a + 1] >> 6) & 0b11;
    rm = p[a + 1] & 0b111;
    get_adm(p, a + 2, mod, rm, ea, &ds);

    switch ((p[a + 1] >> 3) & 0b111) {
    case 0b000: // TODO: special case to fix
      // TEST: imm -> r/m
      print4b(p, a, 3 + w + ds, ip);

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
      print_fw(p, a, "not", ip, ea, LSB4, &ds);
      break;

    case 0b011:
      // NEG
      // printf("mod=%x, rm=%x  ", mod, rm);
      print_fw(p, a, "neg", ip, ea, LSB4, &ds);
      break;

    case 0b100:
      // MUL
      print_fw(p, a, "mul", ip, ea, LSB4, &ds);
      break;

    case 0b101:
      // IMUL
      print_fw(p, a, "imul", ip, ea, LSB4, &ds);
      break;

    case 0b110:
      // DIV
      print_fw(p, a, "div", ip, ea, LSB4, &ds);
      break;

    case 0b111:
      // IDIV
      print_fw(p, a, "idiv", ip, ea, LSB4, &ds);
      break;

    default:
      print_single(p, a, "(undefined)", ip, 1);
      break;
    }
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOCK
      // print_single(p, a + 1, "lock", ip, 1);
      print_single(p, a, "(undefined)", ip, 1);
      break;

    case 0x4:
      // HLT
      print_single(p, a, "hlt", ip, 1);
      break;

    case 0x5:
      // CMC
      print_single(p, a, "cmc", ip, 1);

      break;

    case 0x8:
      // CLC
      // print_single(p, a, "clc", ip, 1);
      print_single(p, a, "(undefined)", ip, 1);
      break;

    case 0x9:
      // STC
      // print_single(p, a, "stc", ip, 1);
      print_single(p, a, "(undefined)", ip, 1);
      break;

    case 0xa:
      // CLI
      print_single(p, a, "cli", ip, 1);
      break;

    case 0xb:
      // STI
      print_single(p, a, "sti", ip, 1);
      break;

    case 0xc:
      // CLD
      print_single(p, a, "cld", ip, 1);
      break;

    case 0xd:
      // STD
      print_single(p, a, "std", ip, 1);
      break;

    case 0xf:
      switch ((p[a + 1] >> 3) & 0b111) {
      case 0b000:
        // INC: r/m
        print_frm(p, a, "inc", ip, ea, LSB4, &ds);
        break;

      case 0b001:
        // DEC: r/m
        print_frm(p, a, "dec", ip, ea, LSB4, &ds);
        break;

      case 0b010:
        // CALL: indw|seg
        print_frm(p, a, "call", ip, ea, LSB4, &ds);
        break;

      case 0b011:
        // CALL: indirect intersegment
        print_frm(p, a, "call (ind inter)", ip, ea, LSB4, &ds);
        break;

      case 0b100:
        // JMP: indw|seg
        print_frm(p, a, "jmp", ip, ea, LSB4, &ds);
        break;

      case 0b101:
        // JMP: indirect intersegment
        print_frm(p, a, "jmp (ind inter)", ip, ea, LSB4, &ds);
        break;

      case 0b110:
        // PUSH: r/m
        print_frm(p, a, "push", ip, ea, LSB4, &ds);
        break;

      default:
        print_single(p, a, "(undefined)", ip, 1);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}