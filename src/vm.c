//vm-> running = 0;  it is done in stack overflow, stackunderflow, unknown opcode, HALT instructions
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "instruction.h"

#define STACK_MAX 1024 //for now keeping stack capacity as 1024
//central error msg system to also print PC value 
static void vm_error(VM *vm, const char *msg) {
    fprintf(stderr, "Runtime error at PC=%u: %s\n", vm->pc, msg);
    vm->running = 0;
    // will replace with vm_error(vm, "division by zero");

}


//adding this part to show execution over stack
static void vm_dump_stack(VM *vm) 
{
    printf("STACK [size=%u]: ", vm->sp);
    for (uint32_t i = 0; i < vm->sp; i++) {
        printf("%d ", vm->stack[i]);
    }
    printf("\n");
}

//basic push pop is done here with proper bound check
static void vm_push(VM *vm, int32_t value) 
{
    if (vm->sp >= vm->stack_capacity) 
    {
        // fprintf(stderr, "Runtime error: stack overflow\n");
        // vm->running = 0;
        vm_error(vm, "stack overflow");

        return;
    }
    vm->stack[vm->sp++] = value; //stack pointer increment happens here
}

static int32_t vm_pop(VM *vm) 
{
    if (vm->sp == 0) 
    {
        // fprintf(stderr, "Runtime error: stack underflow\n");
        // vm->running = 0;
        vm_error(vm, "stack underflow");

        return 0;
    }
    return vm->stack[--vm->sp];
}

//initialise VM here
void vm_init(VM *vm, uint8_t *code, size_t code_size) 
{
    #define GLOBALS_MAX 256 // FOR NOW 256 GLOBALS ONLY
    vm->instr_count = 0;
    vm->globals_count = GLOBALS_MAX;
    vm->globals = calloc(GLOBALS_MAX, sizeof(int32_t)); // DOING CALLOC HERE BEACUSE IT WILL BE A ZERO INITIALIZED MEMORY
    vm->code = code;
    vm->code_size = code_size;
    vm->pc = 0;

    vm->stack_capacity = STACK_MAX;
    vm->stack = malloc(sizeof(int32_t) * STACK_MAX);  //allocating stack to vm here
    vm->sp = 0;
    vm->fp = 0;

    vm->running = 1;
    vm->trace = 0;   // off by default

}

void vm_run(VM *vm) 
{
    while (vm->running) 
    {
        
        //this if below will automatically prevent bad JMP,JZ as well as wrong PC values
        if (vm->pc >= vm->code_size) 
        {
            fprintf(stderr, "Runtime error: PC out of bounds\n");
            break;
        }

        // uint8_t opcode = vm->code[vm->pc++]; 
        uint8_t opcode = vm->code[vm->pc++];// this is fetch , reads instruction and increment PC
        vm->instr_count++;

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

        case OP_SWAP: 
            {
            if (vm->sp < 2) {
                // fprintf(stderr, "Runtime error: SWAP needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "SWAP NEEDS 2 VALUES ATLEAST");

                break;
            }
            int32_t a = vm->stack[vm->sp - 1];
            int32_t b = vm->stack[vm->sp - 2];
            vm->stack[vm->sp - 1] = b;
            vm->stack[vm->sp - 2] = a;
            break;
            }
        case OP_DROP: 
        {
            if (vm->sp == 0) {
                // fprintf(stderr, "Runtime error: DROP on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "DROP on empty stack");

                break;
            }
            vm->sp--;   // discard top
            break;
        }
        case OP_OVER: 
        {
            if (vm->sp < 2) {
                // fprintf(stderr, "Runtime error: OVER needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "OVER needs 2 values");

                break;
            }
            int32_t v = vm->stack[vm->sp - 2];
            vm_push(vm, v);
            break;
        }


        
        //--------------Arithmatic section here ----------------------
        // stack will be LIFO, order is done to mirror ALU semantics a+b and b+a is different for stack
        case OP_ADD: {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a + b);
            break;
        }

        case OP_SUB: {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a - b);
            break;
        }

        case OP_MUL: {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a * b);
            break;
        }

        case OP_DIV: {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
        
            if (b == 0) {
                // fprintf(stderr, "Runtime error: division by zero\n");
                // vm->running = 0;
                vm_error(vm, "division by zero");
                break;
            }
        
        
            vm_push(vm, a / b);
            break;
        }
        case OP_DUP: {
            if (vm->sp == 0) {
                // fprintf(stderr, "Runtime error: DUP on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "DUP on empty stack");

                break;
            }
            int32_t v = vm->stack[vm->sp - 1];
            vm_push(vm, v);
            break;
        }
        case OP_INC: 
        {
            if (vm->sp == 0) {
                // fprintf(stderr, "Runtime error: INC on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "INC on empty stack");

                break;
            }
            vm->stack[vm->sp - 1]++;
            break;
        }
        case OP_DEC: 
        {
            if (vm->sp == 0) {
                // fprintf(stderr, "Runtime error: DEC on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "DEC on empty stack");

                break;
            }
            vm->stack[vm->sp - 1]--;
            break;
        }
        case OP_NEG: 
        {
            if (vm->sp == 0) {
                // fprintf(stderr, "Runtime error: NEG on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "NEG on empty stack");

                break;
            }
            vm->stack[vm->sp - 1] = -vm->stack[vm->sp - 1];
            break;
        }
        //------REL Ops--------
        case OP_EQ: 
        {
            if (vm->sp < 2) {
                // fprintf(stderr, "Runtime error: EQ needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "EQ needs 2 values");
                break;
            }
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, (a == b) ? 1 : 0);
            break;
        }
        case OP_LT:
        {
            if (vm->sp < 2) {
                // fprintf(stderr, "Runtime error: LT needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "LT needs 2 values");

                break;
            }
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, (a < b) ? 1 : 0);
            break;
        }
        case OP_GT: 
        {
            if (vm->sp < 2) {
                // fprintf(stderr, "Runtime error: GT needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "GT needs 2 values");

                break;
            }
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, (a > b) ? 1 : 0);
            break;
        }


        //--------LOAD STORE HERE------------
        case OP_STORE: {
            uint32_t idx = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            if (idx >= vm->globals_count) {
                // fprintf(stderr, "Runtime error: STORE index out of bounds\n");
                // vm->running = 0;
                vm_error(vm, "STORE index out of bounds");

                break;
            }
        
            int32_t val = vm_pop(vm);
            vm->globals[idx] = val;
            break;
        }

        case OP_LOAD: {
            uint32_t idx = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;
        
            if (idx >= vm->globals_count) {
                // fprintf(stderr, "Runtime error: LOAD index out of bounds\n");
                // vm->running = 0;
                vm_error(vm, "LOAD index out of bounds");

                break;
            }
        
            vm_push(vm, vm->globals[idx]);
            break;
        }




        //____________________________________________________________
        //--------control flow here---------
        case OP_JMP: {
            uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;          // consume operand
            vm->pc = addr;        // jump
            break;
        }


        case OP_JZ: {
            // Operand: 4-byte absolute address
            uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;
            
            int32_t cond = vm_pop(vm);
            if (cond == 0) {
                vm->pc = addr;
            }
            //here however operand is part of instruction and if yhe condition is false execution continues after instruction
            //if true PC is overridden
            break;
        }


        //simple stop done to avoid segfault and undefined behavious
        case OP_HALT:
            vm->running = 0; 
            break;

        case OP_PRINT: 
        {
            if (vm->sp == 0) {
                // fprintf(stderr, "Runtime error: PRINT on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "division by zero");

                break;
            }
            int32_t v = vm_pop(vm);
            printf("PRINT: %d\n", v);
            break;
        }
        case OP_NOP:
            // EAT a 5 star and do nothing
            break;



        default:
            fprintf(stderr, "Unknown opcode: 0x%02X\n", opcode);
            vm->running = 0;

        }
        // vm_dump_stack(vm); //added to show execution in stack what happens and how, for later part will guard with a flag
        if (vm->trace) 
        {
            vm_dump_stack(vm);
        }

    }
}

//freeing VM here
void vm_free(VM *vm) 
{
    free(vm->globals);
    free(vm->stack);

}
