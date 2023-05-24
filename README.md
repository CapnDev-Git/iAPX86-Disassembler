# iAPX86 Disassembler

## Table of contents

- [Introduction](#introduction)
- [Compilation](#compilation)
  - [Compilation Librairies](#compilation-librairies)
  - [C Librairies](#c-librairies)
- [Supported instructions](#supported-instructions)
  - [DATA TRANSFER](#data-transfer)
  - [ARITHMETIC](#arithmetic)
  - [LOGIC](#logic)
  - [STRING MANIPULATION](#string-manipulation)
  - [CONTROL TRANSFER](#control-transfer)
  - [PROCESSOR CONTROL](#processor-control)
- [How to use](#how-to-use)
  - [Compilation](#compilation-1)
  - [Execution](#execution)
  - [Testing](#testing)
    - [Unit tests example](#unit-tests-example)
  - [Cleaning](#cleaning)

## Introduction

**_Still work in progress. Not all instructions are supported yet._**

Code in C by FLECHTNER Eliott.

Consists of a disassembler (binary to 8086 ASM) for **iAPX86** (8086 16-BIT HMOS Microprocessor) created by Intel in 1978 (first ever x86 microprocessor).

Developed in the context of [Advanced OS &amp; Virtualization](http://syllabus.sic.shibaura-it.ac.jp/syllabus/2023/din/138807.html?g=101) class by Instructor [**福田 浩章**](http://resea.shibaura-it.ac.jp/?2830ea708a1eddbb7e8bb6c2a366b751) (FUKUDA Hiroaki) at [**芝浦工業大学**](https://www.shibaura-it.ac.jp/en/) (Shibaura Institute of Technology) in 東京都 (Tōkyō-to, Japan).

## Compilation

### Compilation Librairies

This project uses [Makefiles](<https://www.wikiwand.com/en/Make_(software)>) to compile and create an executable.

### C Librairies

No external librairies are necessary except the following C headers:

- ctype.h
- err.h
- stdint.h
- stdio.h
- stdlib.h
- string.h

## Supported instructions

**_Instruction set estimated completion percentage: ~60% (73 / 120)_**

Confer to [documentation](./doc/iAPX86.pdf) for more details on instructions & addressing modes.

**Legend**

- -> = to
- <- = from
- <-> = either
- \+ = with
- & = and
- r/m = Register/Memory
- reg = Register
- acc = Accumulator
- imm = Immediate
- EA = Effective Address
- w| = within
- dir = Direct
- indir = Indirect
- addimm = Adding Immediate
- (un)spec = (Un)Specified

### DATA TRANSFER

- MOV: r/m <-> reg, imm -> r/m, imm -> reg
- PUSH: r/m, reg
- POP: reg
- XCHG: r/m + reg, reg + acc
- IN: fixed/variable port
- LEA: EA -> reg

### ARITHMETIC

- ADD: r/m + reg <->, imm -> r/m
- ADC: r/m + reg <->
- INC: r/m, reg
- SUB: r/m & reg <->, imm <- r/m, imm <- acc
- SSB: r/m & reg <->, imm <- r/m
- DEC: r/m, reg
- NEG
- CMP: r/m & reg, imm + r/m, imm + acc
- MUL
- DIV
- CBW
- CWD

### LOGIC

- NOT
- SHL/SAL
- SHR
- SAR
- RCL
- AND: r/m & reg <->, imm -> r/m
- TEST: r/m & reg, imm & r/m, imm & acc
- OR: r/m & reg <->, imm -> r/m
- XOR: r/m & reg <->

### STRING MANIPULATION

- REP
- MOVS
- CMPS
- SCAS
- LODS
- STOS

### CONTROL TRANSFER

- CALL: dirw|seg, indirw|seg
- JMP: dirw|seg, dirw|seg-short, indirw|seg
- RET: w|seg, w|seg addimm -> SP
- JE/JZ
- JL/JNGE
- JLE/JNG
- JB/JNAE
- JBE/JNA
- JNE/JNZ
- JNL/JGE
- JNLE/JG
- JNB/JAE
- JNBE/JA
- LOOP
- INT: spec, 3

### PROCESSOR CONTROL

- CLD
- STD
- HLT

## How to use

### Compilation

To compile the project, you need to run the following command:

```bash
make # Creates the "disassembler" executable & test/utest executable
```

### Execution

To execute the program, the following syntax should be used:

```bash
# Prints in stdin the disassembled 8086-ASM code of given binary
# file formatted with address number and both hexadecimal & text
# representation of the instructions.
./disassembler <file>

# Prints in stdin the dump in both hexadecimal and binary
# representation of given binary file, no 8086-ASM code is
# printed.
./disassembler [-d|--dump] <file> # Dumping flag: -d or --dump
```

### Testing

The following commands are examples of how to use the disassembler:

```bash
./disassembler -d ./test/execs/1c
./disassembler ./test/execs/1c
```

The executable file located in the `./test/` folder named `utest` can be used as a unit tests suite for the disassembler on the provided test files in `./test/execs`. To use it, you need to run the following command:

```bash
./test/utest [-v|--verbose] <file|directory>
```

The `-v` or `--verbose` flag can be used to print the details of the missmatches between the expected output and the actual output from reference disassembler.

#### Unit tests example

```bash
./test/utest --verbose ./test/execs/2c_a.out # For testing a single file with verbose
./test/utest -v ./test/execs # For testing the entire directory with verbose
```

### Cleaning

To clean the project, you need to run the following command:

```bash
make clean # To clean the project
```
