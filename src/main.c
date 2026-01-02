#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"

//-------------Program 1 basic push, add------------
// simply some machine code is added here later maybe will link to to a file from tests
// uint8_t program[] =
// {
//     0x01, 10, 0, 0, 0,   // PUSH 10
//     0x01, 20, 0, 0, 0,   // PUSH 20
//     0x10,               // ADD
//     0xFF                // HALT
//     /*
//     Program:
//     PUSH 10
//     PUSH 20
//     ADD
//     HALT
//      */

// };
//--------program 2 multiply, div, sub--------------
// uint8_t program[] = {
//     0x01, 20, 0, 0, 0,  // PUSH 20
//     0x01, 5,  0, 0, 0,  // PUSH 5
//     0x11,               // SUB   -> 15
//     0x01, 3,  0, 0, 0,  // PUSH 3
//     0x12,               // MUL   -> 45
//     0x01, 9,  0, 0, 0,  // PUSH 9
//     0x13,               // DIV   -> 5
//     0xFF                // HALT
// };
//-------Program3 basic loop----------
// uint8_t program[] = {
//     0x01, 1, 0, 0, 0,          //PUSH
//     0x01, 1, 0, 0, 0,          //PUSH
//     0x11,                     //SUB
//     0x03,                     //DUP
//     0x21, 22, 0, 0, 0,         //JZ end
//     0x20, 5, 0, 0, 0,          //JMP loop
//     0xFF                      //HALT
// };
//-------pROGRAM 4 MINIMAL LOAD STORE--------
// uint8_t program[] = {
//     0x01, 42, 0, 0, 0,      // PUSH 42
//     0x30, 0,  0, 0, 0,      // STORE 0
//     0x01, 8,  0, 0, 0,      // PUSH 8
//     0x30, 1,  0, 0, 0,      // STORE 1
//     0x31, 0,  0, 0, 0,      // LOAD 0
//     0x31, 1,  0, 0, 0,      // LOAD 1
//     0x10,                  // ADD
//     0xFF                   // HALT
// };
//--------PROGRAM TEST FOR SWAP DROP OVER INC DEC
// uint8_t program[] = {
//     0x01, 10, 0, 0, 0,   // PUSH 10
//     0x01, 20, 0, 0, 0,   // PUSH 20
//     0x04,               // SWAP        -> [20,10]
//     0x06,               // OVER        -> [20,10,20]
//     0x05,               // DROP        -> [20,10]
//     0x17,               // DEC         -> [20,10]
//     0x16,               // INC         -> [20,11]
//     0xFF
// };
//-------program to test push, inc, dec, neg, eq, lt,nop
// uint8_t program[] =
// {
//     0x01, 5, 0, 0, 0,     // PUSH 5
//     0x18,                // NEG        -> -5
//     0x16,                // INC        -> -4
//     0x17,                // DEC        -> -5

//     0x01, 5, 0, 0, 0,     // PUSH 5
//     0x19,                // EQ         -> 1

//     0x01, 3, 0, 0, 0,     // PUSH 3
//     0x1A,                // LT         -> 0 (1 < 3? careful!)

//     0x60,                // PRINT      -> prints result
//     0x00,                // NOP
//     0xFF
// };

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "Error: cannot open '%s'\n", path);
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Error: fseek failed on '%s'\n", path);
        fclose(f);
        return NULL;
    }
    long len = ftell(f);
    if (len < 0)
    {
        fprintf(stderr, "Error: ftell failed on '%s'\n", path);
        fclose(f);
        return NULL;
    }
    rewind(f);

    uint8_t *buf = malloc((size_t)len);
    if (!buf)
    {
        fprintf(stderr, "Error: malloc failed for '%s'\n", path);
        fclose(f);
        return NULL;
    }

    size_t nread = fread(buf, 1, (size_t)len, f);
    if (nread != (size_t)len)
    {
        fprintf(stderr, "Error: short read from '%s'\n", path);
        free(buf);
        fclose(f);
        return NULL;
    }

    fclose(f);
    *out_size = (size_t)len;
    return buf;
}

static void usage(const char *progname)
{
    fprintf(stderr,
            "Usage: %s [--d] program_file\n"
            "  --d           enable trace (vm.trace = 1)\n"
            "  program_file  bytecode program to execute\n",
            progname);
}

int main(int argc, char **argv)
{
    int trace = 0;
    const char *prog_path = NULL;

    // very small hand‑rolled parser: optional --d, then filename
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--d") == 0)
        {
            trace = 1;
        }
        else if (!prog_path)
        {
            prog_path = argv[i];
        }
        else
        {
            fprintf(stderr, "Error: unexpected argument '%s'\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (!prog_path)
    {
        usage(argv[0]);
        return 1;
    }

    size_t prog_size = 0;
    uint8_t *program = read_file(prog_path, &prog_size);
    if (!program)
    {
        return 1;
    }

    VM vm;
    vm_init(&vm, program, prog_size);
    vm.trace = trace ? 1 : 0;
    vm_run(&vm);

    printf("Executed %lu instructions\n", vm.instr_count);

    if (vm.sp > 0)
    {
        printf("Result on stack: %d\n", vm.stack[vm.sp - 1]);
    }

    vm_free(&vm);
    free(program);
    return 0;
}
