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

Assembly test programs are located in `Assets/tests/Atests/`.

### Assemble a Single Program

```bash
./Assets/bin/assembler Assets/tests/Atests/T1.txt
```

This produces a compiled `T1.bin` file in `Assets/tests/Atests/`.

**or**

```bash
./Assets/bin/assembler <INPUT_DIR> -o <OUTPUT_DIR>
```

### Example

```bash
./Assets/bin/assembler Assets/tests/Atests/factorial.txt
# Output: Assets/tests/Atests/factorial.bin
```

---

## Running the Virtual Machine

### Execute a Compiled Program

```bash
./Assets/bin/vm Assets/tests/Atests/Assembler_outputs/T1.bin
```

### Enable Debug Tracing

To see instruction-by-instruction execution with stack dumps, using `--d` flag:

```bash
./vm --d Assets/tests/Atests/Assembler_outputs/T1.bin
```

---

## Running All Provided Tests

A convenience script is provided to assemble and run all test programs if no arguments are passed:

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
./run.sh <INPUT_FILE>
```

**or**

```bash
# Assemble
./Assets/bin/assembler Assets/tests/Atests/factorial.txt

# Execute
./Assets/bin/vm Assets/tests/Atests/Assembler_outputs/factorial.bin

# Execute with debug trace
./Assets/bin/vm --d Assets/tests/Atests/Assembler_outputs/factorial.bin
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
./Assets/bin/vmvm Assets/tests/Atests/T1.bin

OR
# run a single test using provided script
./run.sh Assets/tests/Atests/T1.txt
```
