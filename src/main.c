#include <stdio.h>
#include "vm.h"


int main() 
{
    //simply some machine code is added here later maybe will link to to a file from tests
    uint8_t program[] = 
    {
        0x01, 10, 0, 0, 0,   // PUSH 10
        0x01, 20, 0, 0, 0,   // PUSH 20
        0x10,               // ADD
        0xFF                // HALT
        /*
        Program:
        PUSH 10
        PUSH 20
        ADD
        HALT
         */

    };

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
