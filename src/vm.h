/*
this file is for machine definition 
*/
#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>

//---------- LAB 5 ADD ONs ----------------
//idea is to change VM structure such that we can calssify between value being an obkject or address or simply value
//defining an enum to take care of it
typedef enum {
    VAL_INT,    // integer value
    VAL_OBJ   //    object reference
} ValueType;

// now that the value type is defined we change the structure that stores it
//structure will look somewhat like this

//  --------------------
//  | value   | type   |
//  ---------------------
typedef struct {
    ValueType type;
    union {
        int32_t i;
        void*   obj;   // heap doesnt exist yet so so string it as void for now
    };
} Value;

//-----------------------------------------




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
    // int32_t *stack; //operand stack
    //change of lab 5
    // previously stack was int32_t type now it is Value type
    Value *stack; // operand stack
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
