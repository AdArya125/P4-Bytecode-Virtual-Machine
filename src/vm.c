// vm-> running = 0;  it is done in stack overflow, stackunderflow, unknown opcode, HALT instructions
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "instruction.h"

#define STACK_MAX 1024 // for now keeping stack capacity as 1024
// central error msg system to also print PC value
static void vm_error(VM *vm, const char *msg)
{
    fprintf(stderr, "Runtime error at PC=%u: %s\n", vm->pc, msg);
    vm->running = 0;
    // will replace with vm_error(vm, "division by zero");
}

// adding this part to show execution over stack
// was working fine for single stack architecture but moving to two stack architecture we have to change it 
//static void vm_dump_stack(VM *vm)
// {
//     printf("STACK [size=%u]: ", vm->sp);
//     for (uint32_t i = 0; i < vm->sp; i++)
//     {
//         printf("%d ", vm->stack[i]);
//     }
//     printf("\n");
// }
//updated code for tw stack comes here
static void vm_dump_stack(VM *vm)
{
    printf("DATA STACK [size=%u]: ", vm->ds_sp);
    for (uint32_t i = 0; i < vm->ds_sp; i++)
        printf("%d ", vm->data_stack[i]);
    printf("\n");
}



// basic push pop is done here with proper bound check
//----------------------------------------------------
//single stack push pop which worked fine was here but updating it ahead to two stack architecture
// static void vm_push(VM *vm, int32_t value)
// {
//     if (vm->sp >= vm->stack_capacity)
//     {
//         // fprintf(stderr, "Runtime error: stack overflow\n");
//         // vm->running = 0;
//         vm_error(vm, "stack overflow");

//         return;
//     }
//     vm->stack[vm->sp++] = value; // stack pointer increment happens here
// }

// static int32_t vm_pop(VM *vm)
// {
//     if (vm->sp == 0)
//     {
//         // fprintf(stderr, "Runtime error: stack underflow\n");
//         // vm->running = 0;
//         vm_error(vm, "stack underflow");

//         return 0;
//     }
//     return vm->stack[--vm->sp];
// }
//----------------------------------------------------
// updated push pop for two stack architecture comes here
static void vm_push(VM *vm, int32_t value)
{
    if (vm->ds_sp >= vm->data_capacity) {
        vm_error(vm, "data stack overflow");
        return;
    }
    vm->data_stack[vm->ds_sp++] = value;
}

static int32_t vm_pop(VM *vm)
{
    if (vm->ds_sp == 0) {
        vm_error(vm, "data stack underflow");
        return 0;
    }
    return vm->data_stack[--vm->ds_sp];
}


// initialise VM here
void vm_init(VM *vm, uint8_t *code, size_t code_size)
{

// #include <inttypes.h>
//     for (int i = 0; i < code_size; i++)
//         printf(" %" PRIu8 " \n", code[i]);
#define GLOBALS_MAX 256 // FOR NOW 256 GLOBALS ONLY
    vm->instr_count = 0;
    vm->globals_count = GLOBALS_MAX;
    vm->globals = calloc(GLOBALS_MAX, sizeof(int32_t)); // DOING CALLOC HERE BEACUSE IT WILL BE A ZERO INITIALIZED MEMORY
    vm->code = code;
    vm->code_size = code_size;
    vm->pc = 0;

    // vm->stack_capacity = STACK_MAX;
    // vm->stack = malloc(sizeof(int32_t) * STACK_MAX); // allocating stack to vm here
    // vm->sp = 0;
    // vm->fp = 0;
    vm->data_capacity = STACK_MAX;
    vm->data_stack = calloc(STACK_MAX, sizeof(int32_t));
    vm->ds_sp = 0;
    vm->fp = 0;
    
    vm->call_capacity = STACK_MAX;
    vm->call_stack = calloc(STACK_MAX, sizeof(uint32_t));
    vm->cs_sp = 0;


    vm->running = 1;
    vm->trace = 0; // off by default
}

void vm_run(VM *vm)
{
    while (vm->running)
    {

        // this if below will automatically prevent bad JMP,JZ as well as wrong PC values
        if (vm->pc >= vm->code_size)
        {
            fprintf(stderr, "Runtime error: PC out of bounds\n");
            break;
        }

        // uint8_t opcode = vm->code[vm->pc++];
        uint8_t opcode = vm->code[vm->pc++]; // this is fetch , reads instruction and increment PC
        vm->instr_count++;

        // decode happens below , in modern CPU this happens via microcode and jump tables
        switch (opcode)
        {
        // OPpush works as an execute unit, reads operand, opdate PC and modify stack
        // we have implemented here immidiate addressing as well as Variable length instructions
        case OP_PUSH:
            // operand is next 4 bytes (int32)
            int32_t value = *(int32_t *)&vm->code[vm->pc];
            vm->pc += 4;
            vm_push(vm, value);
            break;
        // code for unified stak is here next up is for two stackimplementation
        // case OP_SWAP:
        // {
        //     if (vm->sp < 2)
        //     {
        //         // fprintf(stderr, "Runtime error: SWAP needs 2 values\n");
        //         // vm->running = 0;
        //         vm_error(vm, "SWAP NEEDS 2 VALUES ATLEAST");

        //         break;
        //     }
        //     int32_t a = vm->stack[vm->sp - 1];
        //     int32_t b = vm->stack[vm->sp - 2];
        //     vm->stack[vm->sp - 1] = b;
        //     vm->stack[vm->sp - 2] = a;
        //     break;
        // }
        // two stack implementation of swap comes here
        case OP_SWAP:
        {
            if (vm->ds_sp < 2) {
                vm_error(vm, "SWAP needs 2 values");
                break;
            }
            int32_t a = vm->data_stack[vm->ds_sp - 1];
            int32_t b = vm->data_stack[vm->ds_sp - 2];
            vm->data_stack[vm->ds_sp - 1] = b;
            vm->data_stack[vm->ds_sp - 2] = a;
            break;
        }

        // case OP_DROP:
        // {
        //     if (vm->sp == 0)
        //     {
        //         // fprintf(stderr, "Runtime error: DROP on empty stack\n");
        //         // vm->running = 0;
        //         vm_error(vm, "DROP on empty stack");

        //         break;
        //     }
        //     vm->sp--; // discard top
        //     break;
        // }
        case OP_DROP:
        {
            if (vm->ds_sp == 0) {
                vm_error(vm, "DROP on empty stack");
                break;
            }
            vm->ds_sp--;
            break;
        }

        // case OP_OVER:
        // {
        //     if (vm->sp < 2)
        //     {
        //         // fprintf(stderr, "Runtime error: OVER needs 2 values\n");
        //         // vm->running = 0;
        //         vm_error(vm, "OVER needs 2 values");

        //         break;
        //     }
        //     int32_t v = vm->stack[vm->sp - 2];
        //     vm_push(vm, v);
        //     break;
        // }
        case OP_OVER:
        {
            if (vm->ds_sp < 2) {
                vm_error(vm, "OVER needs 2 values");
                break;
            }
            vm_push(vm, vm->data_stack[vm->ds_sp - 2]);
            break;
        }


        //--------------Arithmatic section here ----------------------
        // stack will be LIFO, order is done to mirror ALU semantics a+b and b+a is different for stack
        case OP_ADD:
        {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a + b);
            break;
        }

        case OP_SUB:
        {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a - b);
            break;
        }

        case OP_MUL:
        {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);
            vm_push(vm, a * b);
            break;
        }

        case OP_DIV:
        {
            int32_t b = vm_pop(vm);
            int32_t a = vm_pop(vm);

            if (b == 0)
            {
                // fprintf(stderr, "Runtime error: division by zero\n");
                // vm->running = 0;
                vm_error(vm, "division by zero");
                break;
            }

            vm_push(vm, a / b);
            break;
        }
        // case OP_DUP:
        // {
        //     if (vm->sp == 0)
        //     {
        //         // fprintf(stderr, "Runtime error: DUP on empty stack\n");
        //         // vm->running = 0;
        //         vm_error(vm, "DUP on empty stack");

        //         break;
        //     }
        //     int32_t v = vm->stack[vm->sp - 1];
        //     vm_push(vm, v);
        //     break;
        // }
        case OP_DUP:
        {
            if (vm->ds_sp == 0) {
                vm_error(vm, "DUP on empty stack");
                break;
            }
            vm_push(vm, vm->data_stack[vm->ds_sp - 1]);
            break;
        }

        // case OP_INC:
        // {
        //     if (vm->sp == 0)
        //     {
        //         // fprintf(stderr, "Runtime error: INC on empty stack\n");
        //         // vm->running = 0;
        //         vm_error(vm, "INC on empty stack");

        //         break;
        //     }
        //     vm->stack[vm->sp - 1]++;
        //     break;
        // }
        case OP_INC:
        {
            if (vm->ds_sp == 0) {
                vm_error(vm, "INC on empty stack");
                break;
            }
            vm->data_stack[vm->ds_sp - 1]++;
            break;
        }

        // case OP_DEC:
        // {
        //     if (vm->sp == 0)
        //     {
        //         // fprintf(stderr, "Runtime error: DEC on empty stack\n");
        //         // vm->running = 0;
        //         vm_error(vm, "DEC on empty stack");

        //         break;
        //     }
        //     vm->stack[vm->sp - 1]--;
        //     break;
        // }
        case OP_DEC:
        {
            if (vm->ds_sp == 0) {
                vm_error(vm, "DEC on empty stack");
                break;
            }
            vm->data_stack[vm->ds_sp - 1]--;
            break;
        }

        // case OP_NEG:
        // {
        //     if (vm->sp == 0)
        //     {
        //         // fprintf(stderr, "Runtime error: NEG on empty stack\n");
        //         // vm->running = 0;
        //         vm_error(vm, "NEG on empty stack");

        //         break;
        //     }
        //     vm->stack[vm->sp - 1] = -vm->stack[vm->sp - 1];
        //     break;
        // }
        case OP_NEG:
        {
            if (vm->ds_sp == 0)
            {
                // fprintf(stderr, "Runtime error: NEG on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "NEG on empty stack");

                break;
            }
            vm->data_stack[vm->ds_sp - 1] = -vm->data_stack[vm->ds_sp - 1];
            break;
        }
        //------REL Ops--------
        case OP_EQ:
        {
            if (vm->ds_sp < 2)
            {
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
            if (vm->ds_sp < 2)
            {
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
            if (vm->ds_sp < 2)
            {
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
        case OP_STORE:
        {
            uint32_t idx = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            if (idx >= vm->globals_count)
            {
                // fprintf(stderr, "Runtime error: STORE index out of bounds\n");
                // vm->running = 0;
                vm_error(vm, "STORE index out of bounds");

                break;
            }

            int32_t val = vm_pop(vm);
            vm->globals[idx] = val;
            break;
        }

        case OP_LOAD:
        {
            uint32_t idx = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            if (idx >= vm->globals_count)
            {
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
        case OP_JMP:
        {
            uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;   // consume operand
            vm->pc = addr; // jump
            break;
        }

        case OP_JZ:
        {
            // Operand: 4-byte absolute address
            uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            int32_t cond = vm_pop(vm);
            if (cond == 0)
            {
                vm->pc = addr;
            }
            // here however operand is part of instruction and if yhe condition is false execution continues after instruction
            // if true PC is overridden
            break;
        }

        // simple stop done to avoid segfault and undefined behavious
        case OP_HALT:
            vm->running = 0;
            break;

        case OP_PRINT:
        {
            // if (vm->sp == 0)
            if (vm->ds_sp == 0)
            {
                // fprintf(stderr, "Runtime error: PRINT on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "PRINT on empty stack");

                break;
            }
            int32_t v = vm_pop(vm);
            printf("PRINT: %d\n", v);
            break;
        }
        case OP_NOP:
            // EAT a 5 star and do nothing
            break;

        // it was affecting the Abstract binary interface here like what was going in stack was 
        // [arg |return_pc | old_fp ]
        //                        ↑ sp
        //     fp ────────────────┘
        // this caused old fp to be accessed in place of arguments by any function which specifically broke test case for fact (T20)

        // new architecture simply changes one line over the old one and adds argument on the top 
        // [return_pc | old_fp | arg]
        //                        ↑ sp
        //     fp ────────────────┘
        //failed to fix issue so reverted back 
        

//single stack implementation of op call falls here and 1st one is correct version 2nd one is failed try to fix issue
//third one will be new corrected implementation for two stack architecture
        // case OP_CALL: 
        // {
        //     uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
        //     vm->pc += 4;

        //     // Push return address
        //     vm_push(vm, vm->pc);

        //     // Push old frame pointer
        //     vm_push(vm, vm->fp);

        //     // New frame starts here
        //     vm->fp = vm->sp - 2;

        //     // Jump to function
        //     vm->pc = addr;
        //     break;
        // }
        // case OP_CALL:
        // {
        //     uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
        //     vm->pc += 4;

        //     // Save metadata
        //     vm_push(vm, vm->pc);   // return_pc
        //     vm_push(vm, vm->fp);   // old_fp

        //     // fp points to metadata base
        //     vm->fp = vm->sp - 2;

        //     // Hide metadata below stack top
        //     vm->sp = vm->fp;       // <-- KEY LINE

        //     // Jump to function
        //     vm->pc = addr;
        //     break;
        // }
        case OP_CALL:
        {
            uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;
        
            // Save return address
            vm->call_stack[vm->cs_sp++] = vm->pc;
        
            // Save old frame pointer
            vm->call_stack[vm->cs_sp++] = vm->fp;
        
            // New frame starts at current data stack top
            vm->fp = vm->ds_sp;
        
            // Jump
            vm->pc = addr;
            break;
        }
//---------------------------------------------------------------
//same as call 1st is correct for single stack, 3rd is correct for two stack architecture       
// case OP_RET: 
        // {
        //     // 1. Pop return value (callee must have pushed it)
        //     int32_t ret = vm_pop(vm);

        //     // 2. Read saved frame data WITHOUT destroying stack yet
        //     uint32_t old_fp = vm->stack[vm->fp + 1];
        //     uint32_t ret_pc = vm->stack[vm->fp];

        //     // 3. Restore stack pointer to caller frame
        //     vm->sp = vm->fp;

        //     // 4. Restore FP and PC
        //     vm->fp = old_fp;
        //     vm->pc = ret_pc;

        //     // 5. Push return value for caller
        //     vm_push(vm, ret);
        //     break;
        // }
        // case OP_RET: 
        // {
        //     // Pop return value (from visible stack)
        //     int32_t ret = vm_pop(vm);

        //     // Restore metadata visibility
        //     vm->sp = vm->fp + 2;

        //     // Restore saved frame
        //     uint32_t old_fp = vm_pop(vm);
        //     uint32_t ret_pc = vm_pop(vm);

        //     vm->fp = old_fp;
        //     vm->pc = ret_pc;

        //     // Push return value for caller
        //     vm_push(vm, ret);
        //     break;
        // }

        case OP_RET:
        {
            // Drop callee locals
            vm->ds_sp = vm->fp;
        
            // Restore frame pointer
            vm->fp = vm->call_stack[--vm->cs_sp];
        
            // Restore return address
            vm->pc = vm->call_stack[--vm->cs_sp];
            break;
        }

//---------------------------------------------------------------
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

// freeing VM here
void vm_free(VM *vm)
{
    free(vm->globals);
    // free(vm->stack);
    free(vm->data_stack);
    free(vm->call_stack);

}
