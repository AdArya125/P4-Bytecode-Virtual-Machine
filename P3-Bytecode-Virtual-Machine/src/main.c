#include <stdio.h>
#include "vm.h"


int main() 
{
    //-------------Program 1 basic push, add------------
    //simply some machine code is added here later maybe will link to to a file from tests
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
    uint8_t program[] = {
        0x01, 10, 0, 0, 0,   // PUSH 10
        0x01, 20, 0, 0, 0,   // PUSH 20
        0x04,               // SWAP        -> [20,10]
        0x06,               // OVER        -> [20,10,20]
        0x05,               // DROP        -> [20,10]
        0x17,               // DEC         -> [20,10]
        0x16,               // INC         -> [20,11]
        0xFF
    };


    /*
    
    output explanation     : we are dumping stack after every sinngle instruction iteration and not loop iteration here
STACK [size=1]: 3 
STACK [size=2]: 3 1
STACK [size=1]: 2
STACK [size=2]: 2 2
STACK [size=1]: 2
STACK [size=1]: 2
STACK [size=2]: 2 1
STACK [size=1]: 1
STACK [size=2]: 1 1
STACK [size=1]: 1
STACK [size=1]: 1
STACK [size=2]: 1 1
STACK [size=1]: 0
STACK [size=2]: 0 0
STACK [size=1]: 0
STACK [size=1]: 0
Result on stack: 0

STACK [size=1]: 3
After PUSH 3
Stack holds x

STACK [size=2]: 3 1
PUSH 1

Preparing for subtraction

STACK [size=1]: 2
SUB → 3 − 1 = 2

x updated to 2
STACK [size=2]: 2 2
DUP

One copy for condition check, one to keep x

STACK [size=1]: 2
JZ pops condition (2 ≠ 0 → no jump)

STACK [size=1]: 2
JMP back to loop start
Stack intact
    */



    VM vm;
    vm_init(&vm, program, sizeof(program));
    vm_run(&vm); //at this point the PC moves, stack changes and CPU goes into running state
     

    if (vm.sp > 0) 
    {
        printf("Result on stack: %d\n", vm.stack[vm.sp - 1]);
    }

    vm_free(&vm);
    return 0;
}
