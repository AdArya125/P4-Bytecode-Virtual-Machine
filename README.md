# Bytecode Virtual Machine

A stack-based bytecode virtual machine with custom assembler, supporting arithmetic operations, control flow, function calls, recursion, and local/global variable management.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

## Table of Contents

- [Overview](#overview)
- [Build Instructions](#build-instructions)
- [Running the Assembler](#running-the-assembler)
- [Running the Virtual Machine](#running-the-virtual-machine)
- [Running Test Programs](#running-test-programs)
- [VM Architecture](#vm-architecture)
- [Instruction Set Architecture (ISA)](#instruction-set-architecture-isa)
- [Programming Examples](#programming-examples)
- [Design Details](#design-details)
- [Limitations and Future Work](#limitations-and-future-work)

---

## Overview

This project implements a complete bytecode execution environment consisting of:

- **Virtual Machine (VM)**: A stack-based interpreter with support for function calls, recursion, and memory management
- **Assembler**: Converts human-readable assembly programs into executable bytecode
- **Test Suite**: Collection of test programs demonstrating VM capabilities

### Key Features

- Stack-based execution model
- Structured function calls with proper frame management
- Support for recursion and nested calls
- Global and local variable storage
- Comprehensive instruction set (arithmetic, logic, control flow, I/O)
- Safety guarantees with bounds checking
- Debugging support with instruction-level tracing

---

## Build Instructions

### Prerequisites

- **GCC** (or any C99-compatible compiler)
- **Make**
- Linux / WSL / macOS environment recommended

### Build Everything

```bash
make
```

This compiles:

- `vm` — the bytecode virtual machine
- `assembler` — the assembler for `.txt` assembly programs

Both executables will be placed in `Assets/bin/`.

### Clean Build

To remove all compiled binaries and intermediate files:

```bash
make clean
```

---

## Running the Assembler

Assembly programs are located in:

```
Assets/tests/Atests/
```

### Assemble a Single Program

```bash
./Assets/bin/assembler Assets/tests/Atests/T1.txt
```

This produces a compiled `.bin` file in:

```
Assets/tests/Atests/Assembler_outputs/
```

### Example

```bash
./Assets/bin/assembler Assets/tests/Atests/factorial.txt
# Output: Assets/tests/Atests/Assembler_outputs/factorial.bin
```

The assembler reads the assembly source, validates syntax, resolves labels, and generates bytecode that the VM can execute.

---

## Running the Virtual Machine

### Execute a Compiled Program

```bash
./vm Assets/tests/Atests/Assembler_outputs/T1.bin
```

### Enable Debug Tracing

To see instruction-by-instruction execution with stack dumps:

```bash
./vm --d Assets/tests/Atests/Assembler_outputs/T1.bin
```

Debug mode displays:

- Current instruction being executed
- Stack state after each instruction
- Program counter and frame pointer values
- Final execution statistics

### Example Output

```
Executing: PUSH 5
Stack: [5]
Executing: PUSH 3
Stack: [5, 3]
Executing: ADD
Stack: [8]
Executing: PRINT
8
Stack: []
Executing: HALT
Program terminated successfully.
Instructions executed: 5
```

---

## Running Test Programs

### Quick Test Execution

A convenience script is provided to assemble and run all test programs:

```bash
./run.sh
```

This script:

1. Builds the assembler and VM
2. Assembles all test cases from `Assets/tests/Atests/`
3. Executes each test sequentially
4. Displays stack traces and final results
5. Reports instruction counts and execution time

### Available Test Programs

The test suite includes:

- **T1.txt** - Basic arithmetic operations
- **T2.txt** - Stack manipulation (DUP, SWAP, DROP, OVER)
- **T3.txt** - Conditional jumps and control flow
- **T4.txt** - Global variable storage and loading
- **T5.txt** - Simple function calls
- **T6.txt** - Recursive factorial implementation
- **T7.txt** - Deeply nested function calls
- **T8.txt** - Fibonacci sequence (recursive)
- **T9.txt** - Complex control flow with multiple branches
- **T10.txt** - Stress test for stack management

### Run a Specific Test

```bash
# Assemble
./Assets/bin/assembler Assets/tests/Atests/factorial.txt

# Execute
./vm Assets/tests/Atests/Assembler_outputs/factorial.bin

# Execute with debug trace
./vm --d Assets/tests/Atests/Assembler_outputs/factorial.bin
```

---

## VM Architecture

### Execution Model

The VM follows a **stack-based execution model**:

- All computations operate on an explicit operand stack
- Instructions implicitly consume operands from the stack top
- Results are pushed back onto the stack
- Program Counter (PC) drives sequential instruction fetch
- Control-flow instructions modify PC for jumps and calls

### Memory Model

The VM memory consists of three regions:

1. **Code Memory**: Stores bytecode program (indexed by PC)
2. **Stack Memory**: Fixed-size array for operands, call frames, and return values
3. **Global Data Segment**: Fixed-size array of 32-bit integers

### Stack Architecture

**Unified Stack Design**: A single stack stores both operand data and control-flow metadata.

Key components:

- **Stack Pointer (sp)**: Tracks next free slot
- **Frame Pointer (fp)**: Points to current function's activation record base

### Function Call Mechanism

When `CALL` executes:

1. Push return address onto stack
2. Push saved frame pointer
3. Push argument count
4. Update `fp` to new frame base
5. Jump to target function

When `RET` executes:

1. Pop return value
2. Restore saved frame pointer and PC
3. Clean up frame metadata and arguments
4. Push return value for caller

### Safety Guarantees

- **Bounds checking** on all stack operations
- **PC validation** before instruction fetch
- **Stack overflow/underflow detection**
- **Graceful error handling** with clear diagnostics

---

## Instruction Set Architecture (ISA)

The VM implements a byte-oriented instruction set with fixed opcodes.

### Instruction Categories

#### Stack Manipulation

| Instruction | Opcode | Operands | Description |
|------------|--------|----------|-------------|
| `NOP` | `0x00` | — | No operation |
| `PUSH` | `0x01` | `<int32>` | Push immediate value |
| `DUP` | `0x03` | — | Duplicate top of stack |
| `SWAP` | `0x04` | — | Swap top two values |
| `DROP` | `0x05` | — | Remove top of stack |
| `OVER` | `0x06` | — | Copy second value to top |

#### Arithmetic & Logic

| Instruction | Opcode | Operands | Description |
|------------|--------|----------|-------------|
| `ADD` | `0x10` | — | Pop two, push `a + b` |
| `SUB` | `0x11` | — | Pop two, push `a - b` |
| `MUL` | `0x12` | — | Pop two, push `a * b` |
| `DIV` | `0x13` | — | Pop two, push `a / b` |
| `INC` | `0x16` | — | Increment top of stack |
| `DEC` | `0x17` | — | Decrement top of stack |
| `NEG` | `0x18` | — | Negate top of stack |
| `EQ` | `0x19` | — | Push `1` if equal, else `0` |
| `LT` | `0x1A` | — | Push `1` if less-than, else `0` |
| `GT` | `0x1B` | — | Push `1` if greater-than, else `0` |

#### Control Flow

| Instruction | Opcode | Operands | Description |
|------------|--------|----------|-------------|
| `JMP` | `0x20` | `<addr>` | Unconditional jump |
| `JZ` | `0x21` | `<addr>` | Jump if zero |

#### Memory Access

| Instruction | Opcode | Operands | Description |
|------------|--------|----------|-------------|
| `STORE` | `0x30` | `<idx>` | Store to global[idx] |
| `LOAD` | `0x31` | `<idx>` | Load from global[idx] |
| `STORE_LOCAL` | `0x32` | `<idx>` | Store to local arg slot |
| `LOAD_LOCAL` | `0x33` | `<idx>` | Load from local arg slot |

#### Function Calls

| Instruction | Opcode | Operands | Description |
|------------|--------|----------|-------------|
| `CALL` | `0x40` | `<addr> <nargs>` | Call function with args |
| `RET` | `0x41` | — | Return from function |

#### I/O & Control

| Instruction | Opcode | Operands | Description |
|------------|--------|----------|-------------|
| `PRINT` | `0x60` | — | Pop and print value |
| `HALT` | `0xFF` | — | Stop execution |

---

## Programming Examples

### Example 1: Simple Arithmetic

```assembly
; Compute (5 + 3) * 2
PUSH 5
PUSH 3
ADD
PUSH 2
MUL
PRINT
HALT
```

**Output**: `16`

### Example 2: Factorial Function

```assembly
; factorial(n)
; Returns n! using recursion

main:
    PUSH 5          ; Compute factorial(5)
    CALL factorial 1
    PRINT
    HALT

factorial:
    LOAD_LOCAL 0    ; Load argument n
    DUP
    PUSH 1
    EQ              ; Check if n == 1
    JZ recursive_case
    
    ; Base case: return 1
    PUSH 1
    RET

recursive_case:
    LOAD_LOCAL 0    ; Load n
    DUP
    PUSH 1
    SUB             ; Compute n-1
    CALL factorial 1 ; factorial(n-1)
    MUL             ; n * factorial(n-1)
    RET
```

**Output**: `120`

### Example 3: Conditional Logic

```assembly
; Print max(a, b)
main:
    PUSH 10
    PUSH 20
    CALL max 2
    PRINT
    HALT

max:
    LOAD_LOCAL 0    ; Load first argument
    LOAD_LOCAL 1    ; Load second argument
    DUP
    SWAP
    DUP
    GT              ; a > b?
    JZ return_second
    
    ; Return first (a)
    DROP
    RET

return_second:
    ; Return second (b)
    SWAP
    DROP
    RET
```

**Output**: `20`

---

## Design Details

### Instruction Dispatch

The VM uses a **switch-based dispatch** strategy:

- Simple and debuggable
- Direct opcode-to-handler mapping
- Easy to extend with new instructions
- Trade-off: Lower performance vs. jump tables or threaded dispatch

### Call Frame Layout

Each function call creates a stack frame:

```
[arguments...]      ← Pushed by caller
[return_address]    ← fp points here
[saved_fp]
[nargs]
[local_data...]     ← Grows upward
```

The frame pointer (`fp`) always references the return address slot, providing stable access to arguments even as the stack pointer moves.

### Local Variable Access

Arguments are accessed relative to `fp`:

```c
slot = fp - nargs + index
```

This formula allows reliable argument indexing:

- `LOAD_LOCAL 0` retrieves the first argument
- `LOAD_LOCAL 1` retrieves the second argument
- And so on...

### Stack Discipline

The unified stack requires strict calling conventions:

1. Caller pushes arguments in order
2. Caller executes `CALL`
3. Callee uses `LOAD_LOCAL` to access arguments
4. Callee executes `RET` with return value on stack
5. Caller receives return value on stack top

---

## Limitations and Future Work

### Current Limitations

- **Unified stack**: Operand data and control metadata share the same stack
- **Stack overflow**: Deep recursion limited by fixed stack size
- **No heap**: Only stack and global storage available
- **No tail-call optimization**: Recursive calls incur full overhead
- **Fixed memory limits**: Stack and global segments have compile-time size limits
- **No dynamic allocation**: Cannot create dynamic data structures
- **Switch dispatch**: Lower performance than advanced dispatch techniques

### Potential Enhancements

- [ ] Separate operand and call stacks
- [ ] Implement heap with basic allocation primitives
- [ ] Add tail-call optimization
- [ ] Use jump tables or threaded dispatch for faster execution
- [ ] Support for dynamic arrays and strings
- [ ] Implement a simple garbage collector
- [ ] Add debugging metadata (stack traces, breakpoints)
- [ ] JIT compilation for hot code paths
- [ ] Support for floating-point arithmetic
- [ ] Module system with linking support

---

## Project Structure

```
.
├── Assets/
│   ├── bin/
│   │   ├── assembler          # Assembler executable
│   │   └── vm                 # VM executable
│   └── tests/
│       └── Atests/
│           ├── T1.txt         # Test source files
│           ├── T2.txt
│           └── Assembler_outputs/
│               ├── T1.bin     # Compiled bytecode
│               └── T2.bin
├── src/
│   ├── vm.c                   # VM implementation
│   ├── vm.h                   # VM header
│   ├── assembler.c            # Assembler implementation
│   └── assembler.h            # Assembler header
├── Makefile                   # Build configuration
├── run.sh                     # Test runner script
└── README.md                  # This file
```

---

## Contributing

Contributions are welcome! Areas for improvement:

- Additional test programs
- Performance optimizations
- New instructions
- Better error messages
- Documentation improvements

---

## License

This project is licensed under the MIT License - see the LICENSE file for details.

---

## Acknowledgments

Developed as part of a course on compiler design and virtual machines. Special thanks to the course instructors and peers for valuable feedback during development.

---

## Contact

For questions or issues, please open a GitHub issue or contact the maintainers.

**Authors**: Aditya, Chirag Kathpalia  
**Institution**: IIT DELHI