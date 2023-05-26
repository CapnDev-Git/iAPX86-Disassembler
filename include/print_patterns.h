#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "patterns.h"
#include "prints.h"
#include "translate.h"

extern const char *REG8[8];
extern const char *REG16[8];
extern const char *SEGREG[4];

void printsw(const unsigned char *p, size_t a, char *instr, size_t *ip,
             char *ea, unsigned char LSB4, size_t *ds);
void printvw(const unsigned char *p, size_t a, char *instr, size_t *ip,
             unsigned char LSB4, size_t *ds);
void printdw(const unsigned char *p, size_t a, char *instr, size_t *ip,
             unsigned char LSB4, size_t *ds);
void print_immtoacc(const unsigned char *p, size_t a, char *instr,
                    unsigned char w, size_t *ip);
void print_immtorm(const unsigned char *p, size_t a, char *instr, size_t *ip,
                   char *ea, unsigned char LSB4, size_t *ds);
void print_rmtoimm(const unsigned char *p, size_t a, char *instr, size_t *ip,
                   char *ea, unsigned char LSB4, size_t *ds);
void print_rm16(const unsigned char *p, size_t a, char *instr, unsigned char rm,
                size_t *ip);
void print_condjmps(const unsigned char *p, size_t a, char *instr, size_t *ip);
void print_repmanip(const unsigned char *p, size_t a, char *instr, size_t *ip,
                    unsigned char w);
void print_single(const unsigned char *p, size_t a, char *instr, size_t *ip,
                  size_t il);
void print_portfixed(const unsigned char *p, size_t a, char *instr, size_t *ip,
                     unsigned char w);
void print_portvar(const unsigned char *p, size_t a, char *instr, size_t *ip,
                   unsigned char w);
void print_loop(const unsigned char *p, size_t a, char *instr, size_t *ip);
void print_calljmp(const unsigned char *p, size_t a, char *instr, size_t *ip);
void print_stringmanip(const unsigned char *p, size_t a, char *instr,
                       unsigned char w, size_t *ip);
void print_fw(const unsigned char *p, size_t a, char *instr, size_t *ip,
              char *ea, unsigned char LSB4, size_t *ds);
void print_frm(const unsigned char *p, size_t a, char *instr, size_t *ip,
               char *ea, unsigned char LSB4, size_t *ds);
void init_lookups();