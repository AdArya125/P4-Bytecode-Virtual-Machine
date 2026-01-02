/*
 This file is for Instruction Set Architecture (ISA)
 Each opcode will be exactly 1 byte.
 Some instructions are followed by operands in bytecode.
 these opcodes are fixed and no changes will be done later

so i defined operations in instruction.h , instruction.h is not implimentation, its an architecture

in x86 architecture it is defined as 0x01, in ARM it is defined as 0xE1A0
VM, Assembler, tests rely on this
instruction sets are contracts not code

 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>


typedef enum {
    OP_PUSH = 0x01,   // PUSH <int32>
    OP_ADD  = 0x10,   // a + b
    
    OP_SUB  = 0x11,
    OP_MUL  = 0x12,
    OP_DIV  = 0x13,

    OP_HALT = 0xFF
} Opcode;

#endif
