#!/usr/bin/env bash
set -euo pipefail


ASM_DIR="./src/Assembler"
ROOT_DIR="."
TESTS_DIR="./Assets/tests/Atests"  # ← path OK
VM_BINARY="$ROOT_DIR/vm"
ASM_BINARY="./Assets/bin/vm-asm"
GCC_CMD="gcc -Wall -Wextra -std=c11 -g -I. -I.. ./src/Assembler/{lexer,symbols,assembler,asm_main}.c -o $ASM_BINARY"



echo "=== Assembler Tests ==="
echo "Tests: $TESTS_DIR"
echo "VM: $VM_BINARY"
echo

# 1. Build
echo "Building vm-asm..."
bash -c "$GCC_CMD"
echo "✓ Built $ASM_BINARY"
echo


# 2. FIX #1: Enable proper globbing
shopt -s nullglob dotglob  # empty glob → skip, include all files

# 3. FIX #2: Array + specific .txt glob
if ! mapfile -t asm_files < <(find "$TESTS_DIR" -maxdepth 1 -name "*.txt" -print | sort -V); then
    echo "Error scanning test directory"
    exit 1
fi
# Alternative: shopt -s nullglob; asm_files=("$TESTS_DIR"/*.txt)

if [[ ${#asm_files[@]} -eq 0 ]]; then
    echo "No .txt files found in $TESTS_DIR"
    ls -la "$TESTS_DIR" || echo "Directory not found"
    exit 1
fi

echo "Found ${#asm_files[@]} .txt files"
echo
mkdir -p ./Assets/tests/Atests/Assembler_outputs

passed=0
total=0

# 4. Loop over ALL .txt files
for asm_file in "${asm_files[@]}"; do
    base=$(basename "$asm_file" .txt)
    bin_file="./Assets/tests/Atests/Assembler_outputs/${base}.bin"

    echo "=== $base ==="

    echo -e -n "Assemble: \n"
    if output=$("$ASM_BINARY" "$asm_file" -o "$bin_file" 2>&1); then
        echo "$output"
        ((++passed))
    else
        echo "FAILED"
    fi

    ((++total))
done


echo "=== Summary ==="
echo "Assembled successfully: $passed/$total"
(( passed == total )) && echo "✓ ALL GOOD" || echo "✗ SOME FAILED"


# Adjust target name if your binary is not 'vm'
TARGET="./Assets/bin/vm"
TEST_DIR="./Assets/tests/Atests/Assembler_outputs"

# Rebuild from scratch
make clean
make

if [ ! -x "$TARGET" ]; then
    echo "Error: $TARGET not built or not executable"
    exit 1
fi
echo "Running VM on all .bin files in $TEST_DIR ..."

# Collect .bin files in version-aware order
if ! mapfile -t bin_files < <(find "$TEST_DIR" -maxdepth 1 -name "*.bin" -print | sort -V); then
    echo "Error scanning VM test directory"
    exit 1
fi

if [[ ${#bin_files[@]} -eq 0 ]]; then
    echo "No .bin files found in $TEST_DIR"
    exit 1
fi

for f in "${bin_files[@]}"; do
    echo
    echo "===== $f ====="
    $TARGET --d "$f"
    echo "File executed: $f"
    echo -e "\npress Enter to continue"
    read -r _
done
