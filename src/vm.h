/*
this file is for machine definition 
*/
#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>

//this struct will tell values of all parts at a given set of time 
// it is more like if we pause VM at current point what values should we save
typedef struct VM 
{
    /* Code part is defined here */
    uint64_t instr_count;

    uint8_t *code;
    size_t code_size;
    uint32_t pc;

    /* Stack part is defined here */
    int32_t *stack; //operand stack
    uint32_t sp; // next free slot
    uint32_t fp; // frame pointer (unused for now will change later)
    uint32_t stack_capacity;
    
    /* --- Global Data Segment --- */
    int32_t  *globals;
    uint32_t  globals_count;

    /* Execution state */
    int running;
    //for debug of stack states
    int trace;          // 0 = off, 1 = on

} VM;

/* VM lifecycle */
void vm_init(VM *vm, uint8_t *code, size_t code_size);
void vm_run(VM *vm);
void vm_free(VM *vm);

#endif
