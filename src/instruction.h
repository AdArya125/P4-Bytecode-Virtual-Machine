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
    /* Stack manipulation */
    OP_DUP  = 0x03,
    OP_SWAP = 0x04,
    OP_DROP = 0x05,
    OP_OVER = 0x06,


    //-----Arithmatic---------
    OP_ADD  = 0x10,   // a + b
    OP_SUB  = 0x11,
    OP_MUL  = 0x12,
    OP_DIV  = 0x13,
        /* Unary arithmetic */
    OP_INC  = 0x16,
    OP_DEC  = 0x17,
    /* Unary arithmetic */
    OP_NEG   = 0x18,
    
    /* Comparisons */
    OP_EQ    = 0x19,
    OP_LT    = 0x1A,
    OP_GT    = 0x1B,
    
    /* I/O / misc */
    OP_PRINT = 0x60,
    OP_NOP   = 0x00,

    

    //--------control flow-------
    OP_JMP = 0x20, // works as JMP <addr> or JZ <addr>
    OP_JZ = 0x021, 

    //-------Load/store-------
    OP_STORE = 0x30,   // STORE <idx>
    OP_LOAD  = 0x31,   // LOAD  <idx>


    OP_HALT = 0xFF
} Opcode;

#endif
