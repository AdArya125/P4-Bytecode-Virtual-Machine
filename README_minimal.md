# Bytecode Virtual Machine

A stack-based bytecode virtual machine with custom assembler.

## Build Instructions

### Prerequisites
- GCC (or any C99-compatible compiler)
- Make
- Linux / WSL environment recommended

### Build Everything

```bash
make
```

This builds:
- `vm` — the bytecode virtual machine
- `assembler` — the assembler for `.txt` assembly programs

Both executables will be placed in `Assets/bin/`.

---

## Running the Assembler

Assembly programs are located in `Assets/tests/Atests/`.

### Assemble a Single Program

```bash
./Assets/bin/assembler Assets/tests/Atests/T1.txt
```

This produces a compiled `.bin` file in `Assets/tests/Atests/Assembler_outputs/`.

### Example

```bash
./Assets/bin/assembler Assets/tests/Atests/factorial.txt
# Output: Assets/tests/Atests/Assembler_outputs/factorial.bin
```

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

---

## Running All Provided Tests

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

## Quick Start

```bash
# Build everything
make

# Run all tests
./run.sh

# Or run a single test
./Assets/bin/assembler Assets/tests/Atests/T1.txt
./vm Assets/tests/Atests/Assembler_outputs/T1.bin
```
