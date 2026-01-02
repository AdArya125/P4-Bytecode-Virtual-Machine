# P3-Bytecode-Virtual-Machine

instruction.h defines what instruction exists and not how they run that i done in different part
vm.h defines a basic vm and makes vm reusable, it tells what must exist for execution to resume correctly
vm.c : turns bytecode to behaviour, this is for stack micro operations, instruction dispatch loop, runtime error handling
this is where the virtual machine gets implemented

main.c : in 1st pass it has hardcoded bytecode because we are just checking VM and not tooling yet, this heplp avoiding assembler bugs early
what wull this file contain... it is only to drive the VM

currently stack operations and arithmatic is defined in instruction.h
already done part is PUSH and ADD
in vm.c we defined push and add

next up to aditya is to add arithmatic cases, control flow
later I will handle memory and CALL and RET
