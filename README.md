# Systems Programming Lab — Two-Pass Assembler in C

A complete **two-pass assembler** written in C, developed as the final project (ממ"ן 14) for [Course 20465 — Systems Programming Lab](https://www.openu.ac.il/courses/20465.htm) at the Open University of Israel.

> ⚡ Built entirely from scratch **before modern AI coding tools existed** — pure problem-solving, debugging, and self-reliance.

**Authors:** Lucas Rohana, Joseph Soussan  
**Date:** August 2024

---

## Overview

This assembler translates programs written in a custom assembly language into machine code. It processes source files through three stages: macro expansion (pre-assembler), first pass (symbol table construction), and second pass (code generation and output).

## How It Works

### Stage 1 — Pre-Assembler (Macro Expansion)
- Reads the source `.as` file and expands all macro definitions
- Validates macro names (not a reserved word, no duplicates)
- Outputs a `.am` file with all macros expanded
- **Files:** `macros_expand.c`, `macros_help.c`

### Stage 2 — First Pass
- Scans the expanded source file line by line
- Builds the **symbol table** — maps labels to memory addresses
- Encodes instructions and data into binary machine code
- Validates labels, instructions, guidance directives, and operands
- Tracks instruction counter (IC) and data counter (DC)
- **Files:** `first_pass.c`, `passes_help.c`, `instructions_help.c`, `guidance_help.c`

### Stage 3 — Second Pass
- Resolves all label references using the symbol table
- Completes the binary encoding of operands
- Handles `.entry` declarations
- Marks words as Absolute (A), Relocatable (R), or External (E)
- Generates output files if no errors found
- **Files:** `second_pass.c`, `print.c`

## Supported Features

- **16 opcodes:** `mov`, `cmp`, `add`, `sub`, `lea`, `clr`, `not`, `inc`, `dec`, `jmp`, `bne`, `red`, `prn`, `jsr`, `rts`, `stop`
- **4 addressing methods:** Immediate (`#`), Direct (label), Indirect register (`*r`), Direct register (`r`)
- **8 general registers:** r0–r7
- **15-bit word size**, 4096-word memory space
- **2's complement** integer representation
- **Macro expansion** with full validation
- **Comprehensive error detection** with line numbers

## Output Files

| File | Description |
|------|-------------|
| `.am` | Source after macro expansion |
| `.ob` | Object file — machine code in octal encoding |
| `.ent` | Entry points (labels declared with `.entry`) |
| `.ext` | External references (labels declared with `.extern`) |

## Project Structure

```
├── assembler.c          # Main entry point, command-line handling
├── assembler.h          # Header file — structs, macros, function declarations
├── pre_assemb.c         # File handling and pre-assembler orchestration
├── macros_expand.c      # Macro expansion logic
├── macros_help.c        # Macro table management (add, lookup, expand, free)
├── first_pass.c         # First pass — symbol table and initial encoding
├── passes_help.c        # Helper functions for both passes
├── instructions_help.c  # Instruction parsing, operand handling, binary encoding
├── guidance_help.c      # Guidance directives (.data, .string, .entry, .extern)
├── second_pass.c        # Second pass — label resolution and code completion
├── print.c              # Output generation (octal print, entry/extern files)
├── makefile             # Build configuration
└── tests/
    └── test1.am         # Example assembly test file
```

## Build & Run

```bash
make
./assembler file1 file2 file3
```

Input files should have the `.as` extension. The assembler processes each file independently and generates the appropriate output files.

### Example

```bash
./assembler test1
```

This reads `test1.as`, expands macros to `test1.am`, and if no errors are found, produces `test1.ob`, `test1.ent`, and `test1.ext`.

## Technical Highlights

- **Modular architecture** — clean separation of concerns across 10 source files
- **Dynamic memory management** — linked lists for symbol table and macro storage with proper cleanup
- **Robust error handling** — detects and reports all syntax errors with line numbers, continues processing to find multiple errors
- **Bitwise operations** — encodes instructions using bit manipulation for opcode, addressing methods, and A/R/E fields
- **Two-pass design** — first pass builds the symbol table, second pass resolves forward references

## What I Learned

- Low-level memory management in C (malloc, realloc, free, linked lists)
- Designing a multi-pass assembler architecture
- Bitwise operations and binary/octal encoding
- Building and debugging a large C project (10+ source files) independently
- Writing modular, well-documented code with proper header/source separation
- Comprehensive error handling and input validation
- Collaborative development (pair programming)

## Tech Stack

- **Language:** C (ANSI C / C90)
- **Compiler:** gcc with `-Wall -ansi -pedantic` flags
- **Build:** Make
- **Platform:** Linux / Ubuntu

---

*Developed as the final project for Course 20465 — Systems Programming Lab at the Open University of Israel.*
