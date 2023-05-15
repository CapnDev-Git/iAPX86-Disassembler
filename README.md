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

- MOV: r/m <-> reg
- MOV: imm -> reg
- PUSH: reg
- POP: reg
- LEA: EA -> reg
- ADD: r/m + reg <->
- CMP: imm + r/m
- TEST: imm & r/m
- XOR: r/m & reg <->
- CALL: dirseg
- JNE/JNZ
- JNB/JAE
- INT: spec/unspec type

### Compilation

To compile the project, you need to run the following command:

```bash
make
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
./disassembler -dump <file> # Dumping flag: -dump
```

### Tests example

```bash
./disassembler -dump ./tests/1a_c
./disassembler ./tests/1a_c
```

### Cleaning

To clean the project, you need to run the following command:

```bash
make clean # To clean the project
```
