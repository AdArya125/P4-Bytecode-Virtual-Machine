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
static void vm_dump_stack(VM *vm)
{
    printf("STACK [size=%u]: ", vm->sp);
    int flag = (vm->sp > 10) ? 1 : 0;
    if (flag)
    {
        printf(" ... ");
    }
    for (uint32_t i = flag * (vm->sp - 10); i < vm->sp; i++)
    {
        // printf("%d ", vm->stack[i]);
        Value v = vm->stack[i];
        if (v.type == VAL_INT)
            printf("%d ", v.i);
        else
            printf("<obj> ");

    }

    printf("\n");
}

// basic push pop is done here with proper bound check
// --------------- LAB 5 CHANGE HERE ----------------
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
// NOW WE WILL HAVE SEPERATE FUNCTIONS FOR INTEGER PUSH AND OBJ PUSH
static void vm_push_int(VM *vm, int32_t value)
{
    if (vm->sp >= vm->stack_capacity) {
        vm_error(vm, "stack overflow");
        return;
    }
    vm->stack[vm->sp++] = (Value){
        .type = VAL_INT,
        .i = value
    };
}



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

static Value vm_pop(VM *vm)
{
    if (vm->sp == 0) {
        vm_error(vm, "stack underflow");
        return (Value){ .type = VAL_INT, .i = 0 };
        // ABOVE IS CALLED A COMPUND LITRAL (VALUE) {....} AMD .type = VAL_INT, .i = 0 → Designated initializers
        // IT CREATES A TEMPORARY VALUE OBJECT WITH TYPE INT AND VALUE 0
        // IT IS EQUIVALENT TO 
//          Value temp;
//          temp.type = VAL_INT;
//          temp.i = 0;
//          return temp;
//The compound literal avoids the temporary variable entirely.
// AS FOR INITIALIZERS
//      - Initialize specific fields by name
//      - Order does not matter
//      - Unmentioned fields are zero-initialized
// WE CAN ALSO PUT THEN AS  return (Value){ VAL_INT, 0 }; HOWEVER IF STRUCT ORDER CHANGE WE FACE BUGS THATS WHY WE DO DESIGNATED INITIALIZERS
    }
    return vm->stack[--vm->sp];
}

//------------------------------------------------------

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

    vm->stack_capacity = STACK_MAX;
    //lab 5 change here
    //vm->stack = malloc(sizeof(int32_t) * STACK_MAX); // allocating stack to vm here
    // now stack is of Value type
    vm->stack = malloc(sizeof(Value) * STACK_MAX); // allocating stack to vm here
    vm->sp = 0;
    vm->fp = 0;

    vm->running = 1;
    vm->trace = 0; // off by default
    vm->heap.objects = NULL;
    vm->heap.bytes_allocated = 0;
    vm->heap.next_gc = 1024 * 1024 * 4; // assuming 4 MB should be sufficient for now

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
            // vm_push(vm, value);
            // LAB 5 CHANGE HERE
            vm_push_int(vm, value);
            break;

        case OP_SWAP:
        {
            if (vm->sp < 2)
            {
                // fprintf(stderr, "Runtime error: SWAP needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "SWAP NEEDS 2 VALUES ATLEAST");

                break;
            }
            // LAB 5 CHANGE HERE
            // int32_t a = vm->stack[vm->sp - 1];
            // int32_t b = vm->stack[vm->sp - 2];
            // vm->stack[vm->sp - 1] = b;
            // vm->stack[vm->sp - 2] = a;
            Value a = vm->stack[vm->sp - 1];
            Value b = vm->stack[vm->sp - 2];
                    
            vm->stack[vm->sp - 1] = b;
            vm->stack[vm->sp - 2] = a;

            break;
        }
        case OP_DROP:
        {
            if (vm->sp == 0)
            {
                // fprintf(stderr, "Runtime error: DROP on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "DROP on empty stack");

                break;
            }
            vm->sp--; // discard top
            break;
        }
        case OP_OVER:
        {
            if (vm->sp < 2)
            {
                // fprintf(stderr, "Runtime error: OVER needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "OVER needs 2 values");

                break;
            }
            // LAB 5 CHANGE HERE
            // vm_push(vm, v);
            // int32_t v = vm->stack[vm->sp - 2];
            // vm_push_int(vm, v);
            Value v = vm->stack[vm->sp - 2];

            if (v.type != VAL_INT) {
                vm_error(vm, "OVER expects integer");
                break;
            }

            vm_push_int(vm, v.i);

            break;
        }

        //--------------Arithmatic section here ----------------------
        // stack will be LIFO, order is done to mirror ALU semantics a+b and b+a is different for stack
        case OP_ADD:
        {
            //-------------- LAB 5 CHANGE HERE ----------------
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            // vm_push(vm, a + b);
            // CHANGES I HAVE DONE HERE ARE SIMPLE, CHANGED TYPE TO VALUE, ADDED A CHECK TO CONFIRM THEY ARE INT WHEN POPPED
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
                  
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "ADD expects integers");
                break;
            }
            
            vm_push_int(vm, a.i + b.i);

            // ------------------------------------------------

            break;
        }

        case OP_SUB:
        {
            // LAB 5 CHANGE HERE
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "SUB expects integers");
                break;
            }
            // vm_push(vm, a - b);
            vm_push_int(vm, a.i - b.i);
            break;
        }

        case OP_MUL:
        {
            // LAB 5 CHANGE HERE
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "MUL expects integers");
                break;
            }
            // vm_push(vm, a * b);
            // vm_push_int(vm, a * b);
            vm_push_int(vm, a.i * b.i);

            break;
        }

        case OP_DIV:
        {
            // LAB 5 CHANGE HERE
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "... expects integers");
                break;
            }

            if (b.i == 0)
            {
                // fprintf(stderr, "Runtime error: division by zero\n");
                // vm->running = 0;
                vm_error(vm, "division by zero");
                break;
            }

            // vm_push(vm, a / b);
            // LAB 5 CHANGE HERE
            vm_push_int(vm, a.i / b.i );
            break;
        }
        case OP_DUP:
        {
            if (vm->sp == 0)
            {
                // fprintf(stderr, "Runtime error: DUP on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "DUP on empty stack");

                break;
            }
            // int32_t v = vm->stack[vm->sp - 1];
            Value v = vm->stack[vm->sp - 1];
            if (v.type != VAL_INT) {
                vm_error(vm, "expected integer");
                break;
            }
            vm_push_int(vm, v.i);

            // vm_push(vm, v);
            // LAB 5 CHANGE HERE
            // vm_push_int(vm, v);
            break;
        }
        case OP_INC:
        {
            if (vm->sp == 0)
            {
                // fprintf(stderr, "Runtime error: INC on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "INC on empty stack");

                break;
            }
            // vm->stack[vm->sp - 1]++;
            Value *v = &vm->stack[vm->sp - 1];

            if (v->type != VAL_INT) {
                vm_error(vm, "INC/DEC/NEG expects integer");
                break;
            }
            v->i += 1;

            break;
        }
        case OP_DEC:
        {
            if (vm->sp == 0)
            {
                // fprintf(stderr, "Runtime error: DEC on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "DEC on empty stack");

                break;
            }
            // vm->stack[vm->sp - 1]--
            Value *v = &vm->stack[vm->sp - 1];

            if (v->type != VAL_INT) {
                vm_error(vm, "DEC expects integer");
                break;
            }

            v->i -= 1;

            break;
        }
        case OP_NEG:
        {
            if (vm->sp == 0)
            {
                // fprintf(stderr, "Runtime error: NEG on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "NEG on empty stack");

                break;
            }
            // vm->stack[vm->sp - 1] = -vm->stack[vm->sp - 1];
            Value *v = &vm->stack[vm->sp - 1];

            if (v->type != VAL_INT) {
                vm_error(vm, "NEG expects integer");
                break;
            }

            v->i = -v->i;

            break;
        }
        //------REL Ops--------
        case OP_EQ:
        {
            if (vm->sp < 2)
            {
                // fprintf(stderr, "Runtime error: EQ needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "EQ needs 2 values");
                break;
            }
            // LAB 5 CHANGE HERE
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "... expects integers");
                break;
            }
            // vm_push(vm, (a == b) ? 1 : 0);
            // vm_push_int(vm, (a == b) ? 1 : 0);
            vm_push_int(vm, (a.i == b.i) ? 1 : 0);

            break;
        }
        case OP_LT:
        {
            if (vm->sp < 2)
            {
                // fprintf(stderr, "Runtime error: LT needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "LT needs 2 values");

                break;
            }
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "LT expects integers");
                break;
            }
            // vm_push(vm, (a < b) ? 1 : 0);
            // LAB 5 CHANGE HERE
            // vm_push_int(vm, (a < b) ? 1 : 0);
            vm_push_int(vm, (a.i < b.i) ? 1 : 0);

            break;
        }
        case OP_GT:
        {
            if (vm->sp < 2)
            {
                // fprintf(stderr, "Runtime error: GT needs 2 values\n");
                // vm->running = 0;
                vm_error(vm, "GT needs 2 values");

                break;
            }
            // int32_t b = vm_pop(vm);
            // int32_t a = vm_pop(vm);
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type != VAL_INT || b.type != VAL_INT) {
                vm_error(vm, "GT expects integers");
                break;
            }
            // vm_push(vm, (a > b) ? 1 : 0);
            // LAB 5 CHANGE HERE
            // vm_push_int(vm, (a > b) ? 1 : 0);
            vm_push_int(vm, (a.i > b.i) ? 1 : 0);

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

            // int32_t val = vm_pop(vm);
            // Value val = vm_pop(vm);
            // vm->globals[idx] = val;
            Value val = vm_pop(vm);

            if (val.type != VAL_INT) {
                vm_error(vm, "STORE expects integer");
                break;
            }

            vm->globals[idx] = val.i;

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

            // vm_push(vm, vm->globals[idx]);
            // LAB 5 CHANGE HERE
            vm_push_int(vm, vm->globals[idx]);
            break;
        }
        case OP_STORE_LOCAL:
        {
            uint32_t index = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            // int32_t value = vm_pop(vm);
            Value value = vm_pop(vm);
            //check for valid FP
             if (vm->fp + 2 >= vm->sp)
            {
                vm_error(vm, "STORE_LOCAL: corrupted frame pointer");
                break;
            }

            // uint32_t nargs = vm->stack[vm->fp + 2];
            Value nargs_val = vm->stack[vm->fp + 2];
            if (nargs_val.type != VAL_INT) {
                vm_error(vm, "corrupted frame");
                break;
            }
            uint32_t nargs = nargs_val.i;


            if (index >= nargs)
            {
                vm_error(vm, "STORE_LOCAL: arg index out of range");
                break;
            }

            uint32_t slot = vm->fp - nargs + index;
            //added slot bound check
            if (slot >= vm->stack_capacity) 
            {
                vm_error(vm, "STORE_LOCAL: slot out of bounds");
                break;
            }
            vm->stack[slot] = value;
            // printf("[STL ] idx=%d -> addr=%d (FP=%d)\n", idx, vm->fp + LOCAL_BASE + idx, vm->fp);
            printf("[STL ] index=%u -> slot=%u (FP=%u, nargs=%u)\n", index, slot, vm->fp, nargs);

            break;
        }

        case OP_LOAD_LOCAL:
        {
            uint32_t index = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;
     //       FP validity check  
                if (vm->fp + 2 >= vm->sp) {
                    vm_error(vm, "LOAD_LOCAL: corrupted frame pointer");
                    break;
                }

            // uint32_t nargs = vm->stack[vm->fp + 2];
            Value nargs_val = vm->stack[vm->fp + 2];
            if (nargs_val.type != VAL_INT) {
                vm_error(vm, "corrupted frame");
                break;
            }
            uint32_t nargs = nargs_val.i;


            if (index >= nargs)
            {
                vm_error(vm, "LOAD_LOCAL: arg index out of range");
                break;
            }

            uint32_t slot = vm->fp - nargs + index;
            //slots bound check
            if (slot >= vm->stack_capacity) 
            {
                vm_error(vm, "LOAD_LOCAL: slot out of bounds");
                break;
            }
            // vm_push(vm, vm->stack[slot]);
            // LAB 5 CHANGE HERE
            // vm_push_int(vm, vm->stack[slot]);
            Value v = vm->stack[slot];

            if (v.type != VAL_INT) {
                vm_error(vm, "LOAD_LOCAL expects integer");
                break;
            }

            vm_push_int(vm, v.i);

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

            // int32_t cond = vm_pop(vm);
            // Value cond = vm_pop(vm);
            // if (cond == 0)
            // {
            //     vm->pc = addr;
            // }
            Value cond = vm_pop(vm);

            if (cond.type != VAL_INT) {
                vm_error(vm, "JZ expects integer");
                break;
            }

            if (cond.i == 0) {
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
            if (vm->sp == 0)
            {
                // fprintf(stderr, "Runtime error: PRINT on empty stack\n");
                // vm->running = 0;
                vm_error(vm, "division by zero");

                break;
            }
            Value v = vm_pop(vm);
            // printf("PRINT: %d\n", v);
            if (v.type != VAL_INT) {
                vm_error(vm, "PRINT expects integer");
                break;
            }

            printf("PRINT: %d\n", v.i);

            break;
        }
        case OP_NOP:
            // EAT a 5 star and do nothing
            break;

        case OP_CALL:
        {
            uint32_t addr = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            uint32_t nargs = *(uint32_t *)&vm->code[vm->pc];
            vm->pc += 4;

            // 1. push return address
            // vm_push(vm, vm->pc);
            // LAB 5 CHANGE HERE
            vm_push_int(vm, vm->pc);

            // 2. push old FP
            // vm_push(vm, vm->fp);
            // LAB 5 CHANGE HERE
            vm_push_int(vm, vm->fp);

            // 3. push nargs (frame-local)
            // vm_push(vm, nargs);
            // LAB 5 CHANGE HERE           
            vm_push_int(vm, nargs);

            if (!vm->running)
            break;

            // 4. establish new frame (FP → return PC)
            vm->fp = vm->sp - 3;

            // 5. jump
            vm->pc = addr;
            printf("[CALL] FP=%d SP=%d locals=%d\n", vm->fp, vm->sp, nargs);

            break;
        }

        case OP_RET:
        {
            if (vm->fp + 2 >= vm->sp) {
            vm_error(vm, "RET: corrupted frame pointer");
            break;
        }
            Value ret = vm_pop(vm);

            // uint32_t nargs = vm->stack[vm->fp + 2];
            Value nargs_val = vm->stack[vm->fp + 2];
            if (nargs_val.type != VAL_INT) {
                vm_error(vm, "corrupted frame");
                break;
            }
            uint32_t nargs = nargs_val.i;

            // uint32_t old_fp = vm->stack[vm->fp + 1];
            // uint32_t ret_pc = vm->stack[vm->fp];
            Value old_fp_val = vm->stack[vm->fp + 1];
            Value ret_pc_val = vm->stack[vm->fp];

            if (old_fp_val.type != VAL_INT || ret_pc_val.type != VAL_INT) {
                vm_error(vm, "corrupted frame");
                break;
            }

            uint32_t old_fp = old_fp_val.i;
            uint32_t ret_pc = ret_pc_val.i;


            // remove frame + arguments
            vm->sp = vm->fp - nargs;

            // restore control state
            vm->fp = old_fp;
            vm->pc = ret_pc;

            // return value
            // vm_push(vm, ret);
            // LAB 5 CHANGE HERE
            // vm_push_int(vm, ret);
            if (ret.type != VAL_INT) {
                vm_error(vm, "RET expects integer return");
                break;
            }

            vm_push_int(vm, ret.i);

            printf("[RET ] FP=%d SP=%d\n", vm->fp, vm->sp);

            break;
        }

        default:
            fprintf(stderr, "Unknown opcode: 0x%02X\n", opcode);
            vm->running = 0;
        }
        // vm_dump_stack(vm); //added to show execution in stack what happens and how, for later part will guard with a flag
        if (vm->trace)
        {

            printf("(PC : %d) : ", vm->pc);
            vm_dump_stack(vm);
        }
    }
}

// freeing VM here
void vm_free(VM *vm)
{
    // free heap objects
    Obj *obj = vm->heap.objects;
    while (obj) {
        Obj *next = obj->next;
        free(obj);
        obj = next;
    }

    free(vm->globals);
    free(vm->stack);
}
