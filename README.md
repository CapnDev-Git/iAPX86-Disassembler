# iAPX86 Disassembler

Still work in progress.

Code in C by FLECHTNER Eliott.

Consists of a disassembler (binary to 8086 ASM) for **iAPX86** (8086 16-BIT HMOS Microprocessor) created by Intel in 1978 (first ever x86 microprocessor).

Developed in the context of [Advanced OS &amp; Virtualization](http://syllabus.sic.shibaura-it.ac.jp/syllabus/2023/din/138807.html?g=101) class by Instructor [**福田 浩章**](http://resea.shibaura-it.ac.jp/?2830ea708a1eddbb7e8bb6c2a366b751) (FUKUDA Hiroaki) at [**芝浦工業大学**](https://www.shibaura-it.ac.jp/en/) (Shibaura Institute of Technology) in 東京都 (Tōkyō-to, Japan).

## How to use

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

### Supported instructions (so far)

Confer to documentation for more information (./doc/iAPX86.pdf).

**DATA TRANSFER:**

- MOV: r/m <-> reg, imm -> reg
- PUSH: r/m, reg
- POP: reg
- IN: fixed port, variable port
- LEA: EA -> reg

**ARITHMETIC:**

- ADD: r/m + reg <->, imm -> r/m
- SUB: imm <- r/m
- SSB: r/m & reg <->
- DEC: reg
- NEG
- CMP: imm + r/m

**LOGIC:**

- NOT
- SHL/SAL
- AND: r/m & reg <->
- TEST: imm & r/m
- OR: r/m & reg <->
- XOR: r/m & reg <->

**CONTROL TRANSFER:**

- CALL: dirw|seg, indirw|seg
- JMP: dirw|seg, dirw|seg-short
- RET: w|seg
- JE/JZ
- JL/JNGE
- JNE/JNZ
- JNL/JGE
- JNB/JAE
- INT: spec, 3

**PROCESSOR CONTROL:**

- HLT

### Compilation

To compile the project, you need to run the following command:

```bash
make # Creates the "disassembler" executable & "test/utest" executable
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
./disassembler <-d|--dump> <file> # Dumping flag: -d or --dump
```

### Tests example

The following commands are examples of how to use the program:

```bash
./disassembler -d ./test/execs/1a_c
./disassembler ./test/execs/1a_c
```

The executable file located in `./test/utest` folder after compiling the poject with make can be used as a unit tests for the disassembler on the provided test files in `./test/execs`. To use it, you need to run the following command:

```bash
./test/utest [-v|--verbose] <file|directory>
```

The `-v` or `--verbose` flag can be used to print the disassembled code of the test files.

## Unit tests example

```bash
./test/utest --verbose ./test/execs/2c_a.out # For testing a single file with verbose
./test/utest -v ./test/execs # For testing the entire directory with verbose
```

### Cleaning

To clean the project, you need to run the following command:

```bash
make clean # To clean the project
```
