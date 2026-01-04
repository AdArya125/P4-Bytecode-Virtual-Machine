#!/usr/bin/env bash
set -euo pipefail

# Adjust target name if your binary is not 'vm'
TARGET="./vm"
TEST_DIR="tests"

# Rebuild from scratch
make clean
make

if [ ! -x "$TARGET" ]; then
    echo "Error: $TARGET not built or not executable"
    exit 1
fi

echo "Running VM on all .txt files in $TEST_DIR ..."
for f in "$TEST_DIR"/*.bin; do
    [ -e "$f" ] || continue
    echo
    echo "===== $f ====="
    # no trace
    # $TARGET "$f"
    # with trace
    # echo "--- with --d ---"
    $TARGET --d "$f"
    echo "File executed: $f"
    echo -e "\npress Enter to continue"
    read x
done
