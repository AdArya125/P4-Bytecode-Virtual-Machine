#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "instruction.h"

#define STACK_MAX 1024 //for now keeping stack capacity as 1024


//basic push pop is done here with proper bound check
static void vm_push(VM *vm, int32_t value) 
{
    if (vm->sp >= vm->stack_capacity) 
    {
        fprintf(stderr, "Runtime error: stack overflow\n");
        vm->running = 0;
        return;
    }
    vm->stack[vm->sp++] = value; //stack pointer increment happens here
}

static int32_t vm_pop(VM *vm) 
{
    if (vm->sp == 0) 
    {
        fprintf(stderr, "Runtime error: stack underflow\n");
        vm->running = 0;
        return 0;
    }
    return vm->stack[--vm->sp];
}

//initialise VM here
void vm_init(VM *vm, uint8_t *code, size_t code_size) 
{
    vm->code = code;
    vm->code_size = code_size;
    vm->pc = 0;

    vm->stack_capacity = STACK_MAX;
    vm->stack = malloc(sizeof(int32_t) * STACK_MAX);
    vm->sp = 0;
    vm->fp = 0;

    vm->running = 1;
}

void vm_run(VM *vm) 
{
    while (vm->running) 
    {
        if (vm->pc >= vm->code_size) 
        {
            fprintf(stderr, "Runtime error: PC out of bounds\n");
            break;
        }

        uint8_t opcode = vm->code[vm->pc++]; // this is fetch , reads instruction and increment PC
        
        //decode happens below , in modern CPU this happens via microcode and jump tables
        switch (opcode) 
        {
        //OPpush works as an execute unit, reads operand, opdate PC and modify stack
        //we have implemented here immidiate addressing as well as Variable length instructions
        case OP_PUSH: 
            // operand is next 4 bytes (int32)
            int32_t value = *(int32_t *)&vm->code[vm->pc];
            vm->pc += 4;
            vm_push(vm, value);
            break;
        
        // stack will be LIFO, order is done to mirror ALU semantics a+b and b+a is different for stack
        case OP_ADD: 
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a + b);
            break;
        
        //simple stop done to avoid segfault and undefined behavious
        case OP_HALT:
            vm->running = 0;
            break;

        default:
            fprintf(stderr, "Unknown opcode: 0x%02X\n", opcode);
            vm->running = 0;
        }
    }
}

void vm_free(VM *vm) {
    free(vm->stack);
}
