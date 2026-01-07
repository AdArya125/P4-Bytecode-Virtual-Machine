#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="."
BIN_DIR="./Assets/bin"

ASM_BINARY="$BIN_DIR/assembler"
VM_BINARY="$BIN_DIR/vm"

TESTS_DIR="./Assets/tests/Atests"
OUT_DIR="$TESTS_DIR/Assembler_outputs"

# Check if a specific test file was provided
if [[ $# -ge 1 ]]; then
    SPECIFIC_TEST="$1"
    
    # Validate the test file exists
    if [[ ! -f "$SPECIFIC_TEST" ]]; then
        echo "Error: Test file not found: $SPECIFIC_TEST"
        exit 1
    fi
    
    # Validate it's a .txt file
    if [[ "$SPECIFIC_TEST" != *.txt ]]; then
        echo "Error: Test file must be a .txt file: $SPECIFIC_TEST"
        exit 1
    fi
    
    echo "=== Build ==="
    make clean
    make
    echo "Build complete"
    echo
    
    # Sanity checks
    if [[ ! -x "$ASM_BINARY" ]]; then
        echo "Error: assembler not found or not executable: $ASM_BINARY"
        exit 1
    fi
    
    if [[ ! -x "$VM_BINARY" ]]; then
        echo "Error: VM not found or not executable: $VM_BINARY"
        exit 1
    fi
    
    echo "Assembler: $ASM_BINARY"
    echo "VM:        $VM_BINARY"
    echo
    
    mkdir -p "$OUT_DIR"
    
    echo "=== Running File ==="
    
    base=$(basename "$SPECIFIC_TEST" .txt)
    bin_file="$OUT_DIR/$base.bin"
    
    echo "Test: $base"
    echo "Source: $SPECIFIC_TEST"
    echo "Output: $bin_file"
    echo
    echo "Assembling..."
    echo
    
    if output=$("$ASM_BINARY" "$SPECIFIC_TEST" -o "$bin_file" 2>&1); then
        echo "$output"
        echo
        echo "- Assembly successful!"
        echo
        echo "=== Running VM ==="
        echo
        "$VM_BINARY" "$bin_file"
        echo
        echo "Test completed successfully"
        exit 0
    else
        echo "FAILED to assemble"
        echo "$output"
        exit 1
    fi
fi

# If we reach here, no arguments were provided - run all tests
echo "=== Build ==="
make clean
make
echo "Build complete"
echo

# Sanity checks
if [[ ! -x "$ASM_BINARY" ]]; then
    echo "Error: assembler not found or not executable: $ASM_BINARY"
    exit 1
fi

if [[ ! -x "$VM_BINARY" ]]; then
    echo "Error: VM not found or not executable: $VM_BINARY"
    exit 1
fi

echo "Assembler: $ASM_BINARY"
echo "VM:        $VM_BINARY"
echo "Tests:     $TESTS_DIR"
echo

mkdir -p "$OUT_DIR"

echo "=== Running All Tests ==="
echo

# Collect .txt assembly tests
if ! mapfile -t asm_files < <(find "$TESTS_DIR" -maxdepth 1 -name "*.txt" -print | sort -V); then
    echo "Error scanning test directory"
    exit 1
fi

if [[ ${#asm_files[@]} -eq 0 ]]; then
    echo "No .txt files found in $TESTS_DIR"
    exit 1
fi

echo "=== Assembler Tests ==="
echo "Found ${#asm_files[@]} tests"
echo

passed=0
total=0

for asm_file in "${asm_files[@]}"; do
    base=$(basename "$asm_file" .txt)
    bin_file="$OUT_DIR/$base.bin"

    echo "=== $base ==="
    echo "Assembling → $bin_file"

    if output=$("$ASM_BINARY" "$asm_file" -o "$bin_file" 2>&1); then
        echo "$output"
        ((++passed))
    else
        echo "FAILED"
        echo "$output"
    fi

    ((++total))
    echo
done

echo "=== Assembly Summary ==="
echo "Assembled successfully: $passed/$total"
(( passed == total )) && echo "✓ ALL GOOD" || echo "✗ SOME FAILED"
echo

# Stop early if assembler failed
if (( passed != total )); then
    echo "Aborting VM run due to assembler failures"
    exit 1
fi

echo "=== Running VM on all .bin files ==="

# Collect .bin files
if ! mapfile -t bin_files < <(find "$OUT_DIR" -maxdepth 1 -name "*.bin" -print | sort -V); then
    echo "Error scanning VM test directory"
    exit 1
fi

if [[ ${#bin_files[@]} -eq 0 ]]; then
    echo "No .bin files found in $OUT_DIR"
    exit 1
fi

for f in "${bin_files[@]}"; do
    echo
    echo "===== $f ====="
    "$VM_BINARY" --d "$f"
    echo "Executed: $f"
    echo
    read -rp "Press Enter to continue..."
done
