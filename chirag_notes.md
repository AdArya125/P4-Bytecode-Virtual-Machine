TODO
instruction set definition
stack-based execution
call frames and returns
bytecode loader
assembler/bytecode generator

Must have deterministic execution and No memory leaks

Optional : JIT compilation, standard library

My view of Lab 4 

it is like building a CPU in a software like a runtime engine
it will have 4 main parts
    - instructiob format 
    - execution 
    - memory modelling
    - tooling

┌──────────┐
│ Bytecode │  <-- array of instructions (PC walks here)
└────┬─────┘
     │
┌────▼─────┐
│   VM     │
│  Engine  │
└────┬─────┘
     │
 ┌───▼───┐   ┌────────┐   ┌──────────┐
 │ Stack │   │ Memory │   │ CallStack│
 └───────┘   └────────┘   └──────────┘
stack - operands, temps, results
memory is indexed variables
call stack -> return address

file structure plan

lab4-vm/
├── src/
│   ├── vm.c          // execution loop
│   ├── vm.h
│   ├── stack.c
│   ├── stack.h
│   ├── instruction.h
│   ├── loader.c      // bytecode loader
│   └── assembler.c   // text → bytecode
├── tests/
├── docs/
└── Makefile

Till the time aditya is not present I will work on core VM so aditya can add features later 


so phase 1 : ISA design .... basically will do in a enum .... what to read why not macros or why opcodes are grouped in range 0x10 for arithmatic, 0x20 controlflow

phase 2 : Stack implementation to do will be push pop peek overflow, underflow 
to read about why stack based VM makes simpler instruction encoding, why bounds checks are mandatory

phase 3 : Execution phase t study why pc increments sometimes and sometimes not, why stack order matters like b popped before a,how it helps in deterministic execution 

phase 4 : will define a global memory for load and store idx , call stack is also there that we need to implement... 
to study : why return addresses must not go on data stacks and difference bw operand stack and congtrol stack

phase 5 : assembler
text to bytecode conversion will be done here
will map mnemonics to opcode
parse operands if present
emit instruction struct

phase 6 : final testing and documentation
tests must do : arithmatic operation check, divide by zero, conditional jump, simple function call, invalid opc


Additional info : I plan to implement dual stack initially then unified stack FP, SP {will implement it after arithmetic + control flow}

Few questions for myself

    \ Why stack VM over register VM?
    \ Why PC must be updated carefully?
    \ Why SP and FP must be separate?
    \ Why code and data memory separation matters?
    \ Why CALL cannot store return address in a single variable?
Need to lookinto cache locality, branch prediction, memory alignment


Stack based architecure 
    - has no GPRs, all computation on stack
    - operands of any operation implicitly in topof stack
    - simple codes and zero address instructions
    - hardware independence it assumes stack is infinite depth and does not care about number of registers
    Disadvantage : more number of instructions required to perform any task, 2 it can create bottleneck if host cpu's cache is not utilized effectively
     

Register Based architecture 
    - close to no data movementt for temporary data 
    - JIT is just in time compilation, register based have high performance when using this
    Disadvantage : more complex, compiler must perform register allocation (may need graph colouring allocation)

by reducing need of register allocation we can focus on memory layout, 


Does stack implementation means there is no need of registers? --> NO
it still needs registers like : program counter/instruction pointer, stack pointer, frame pointer, instruction register

meomory level architecture 
vonn neumann : same meory instruction and data ...dangerous for normal simple VM systems
harvard : seperate memory space for instructions and data 

we will use modified harvard architecture 
    - Why stack VM over register VM?
    - Why PC must be updated carefully?
    - Why SP and FP must be separate?
    - Why code and data memory separation matters?
    - Why CALL cannot store return address in a single variable?
we will use uint8_t to store code
int32_t array for stack and global variables


designing VM struct, what it should have 

VM {
    code, code size, pc [code will bein uint8_t, codesize in size_t, uint32_t for pc]
    code owill be read only , uint8_t is used to ensure byte level instruction format
    code size will be for bounds checking and loader validation
    pc is unsigned : negative pc should be error so 0<=pc< codesize


    stack, sp,fp [int32_t *stack, uint32_t sp,fp,stack_cap  ]
    stack will be used to hold temporary values, return address, FP, locals
    above is for unified stack design will implement 2 stack idea first

    globals
    running
}


what we did in 1st commit
so built a minimal VM hat can read instructions from memory, maintain execution state and manupulate stack, also stop deterministically

