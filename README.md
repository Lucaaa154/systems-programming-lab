# Systems Programming Lab — Two-Pass Assembler in C

A complete **two-pass assembler** written in C, developed as the final project (ממ"ן 14) for [Course 20465 — Systems Programming Lab](https://www.openu.ac.il/courses/20465.htm) at the Open University of Israel.

> ⚡ Built entirely from scratch **before modern AI coding tools existed** — pure problem-solving, debugging, and self-reliance.

---

## Overview

This assembler translates programs written in a custom assembly language into machine code. It processes source files through multiple stages: macro expansion, two-pass assembly, and output generation.

## Features

### Pre-Assembler (Macro Processor)
- Parses macro definitions (`macr` / `endmacr`)
- Expands macro calls throughout the source code
- Outputs a `.am` file with all macros expanded

### First Pass
- Builds the **symbol table** — maps labels to memory addresses
- Encodes the first word of each instruction
- Processes `.data` and `.string` directives
- Handles `.entry` and `.extern` declarations
- Tracks instruction counter (IC) and data counter (DC)

### Second Pass
- Completes machine code encoding using the symbol table
- Resolves all label references in operands
- Marks words as Absolute (A), Relocatable (R), or External (E)
- Detects and reports errors with line numbers

### Output Files
- **`.ob`** — Object file with machine code in octal encoding
- **`.ent`** — Entry points file (labels declared with `.entry`)
- **`.ext`** — External references file (labels declared with `.extern`)

## Architecture

The assembler supports:
- **16 opcodes**: `mov`, `cmp`, `add`, `sub`, `lea`, `clr`, `not`, `inc`, `dec`, `jmp`, `bne`, `red`, `prn`, `jsr`, `rts`, `stop`
- **4 addressing methods**: Immediate (#), Direct (label), Indirect register (*r), Direct register (r)
- **8 general registers**: r0–r7
- **15-bit word size**, 4096-word memory
- **2's complement** integer representation

## Project Structure

```
├── assembler.c       # Main entry point, command-line handling
├── pre_assembler.c   # Macro expansion (pre-assembler stage)
├── first_pass.c      # First pass — symbol table construction
├── second_pass.c     # Second pass — code generation
├── symbol_table.c    # Symbol table operations
├── parser.c          # Line parsing and validation
├── utils.c           # Helper functions
├── *.h               # Header files
├── makefile           # Build with gcc -Wall -ansi -pedantic
└── tests/             # Test assembly files (.as)
```

> **Note:** The actual file structure may vary slightly — will be updated when project files are added.

## Build & Run

```bash
make
./assembler file1 file2 file3
```

The assembler reads `.as` source files and produces output files (`.am`, `.ob`, `.ent`, `.ext`) for each input file.

## Example

Given an input file `ps.as`, running `./assembler ps` will generate:
- `ps.am` — Source after macro expansion
- `ps.ob` — Object file with machine code
- `ps.ent` — Entry points (if `.entry` directives exist)
- `ps.ext` — External references (if `.extern` directives exist)

## What I Learned

- Low-level memory management in C (malloc, free, pointer arithmetic)
- Designing a multi-pass assembler architecture
- Bitwise operations and binary/octal encoding
- Building and debugging a large C project independently
- Writing modular, well-documented code with proper header/source separation
- Comprehensive error handling and input validation

## Tech Stack

- **Language:** C (C90 / ANSI C)
- **Compiler:** gcc with `-Wall -ansi -pedantic` flags
- **Build:** Make
- **Platform:** Linux / Ubuntu

---

*Developed as the final project for Course 20465 — Systems Programming Lab at the Open University of Israel.*
