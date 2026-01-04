#!/usr/bin/env bash
set -euo pipefail

ASM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$ASM_DIR/.." && pwd)"
TESTS_DIR="$ROOT_DIR/../tests/Atests"  # ← path OK
VM_BINARY="$ROOT_DIR/vm"
ASM_BINARY="$ASM_DIR/vm-asm"
GCC_CMD="gcc -Wall -Wextra -std=c11 -g -I. -I.. {lexer,symbols,assembler,asm_main}.c -o $ASM_BINARY"

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
mapfile -t asm_files < <(
    find "$TESTS_DIR" -maxdepth 1 -name "*.txt" -print |
    sort -V
)
# Alternative: shopt -s nullglob; asm_files=("$TESTS_DIR"/*.txt)

if [[ ${#asm_files[@]} -eq 0 ]]; then
    echo "No .txt files found in $TESTS_DIR"
    ls -la "$TESTS_DIR" || echo "Directory not found"
    exit 1
fi

echo "Found ${#asm_files[@]} .txt files"
echo

passed=0
total=0

# 4. Loop over ALL .txt files
for asm_file in "${asm_files[@]}"; do
    base=$(basename "$asm_file" .txt)
    bin_file="$ASM_DIR/outputs/${base}.bin"

    echo "=== $base ==="

    echo -e -n "Assemble: \n"
    if output=$("$ASM_BINARY" "$asm_file" -o "$bin_file" 2>/dev/null); then
        echo "$output"
        ((++passed))
    else
        echo "FAILED"
    fi

    ((++total))
done


echo "=== Summary ==="
echo "Passed: $passed/$total"
(( passed == total )) && echo "✓ ALL GOOD" || echo "✗ SOME FAILED"
