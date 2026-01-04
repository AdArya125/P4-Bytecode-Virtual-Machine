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


update 
## Stack Design Evolution

### Background

In the initial implementation of the bytecode virtual machine, a **unified stack model** was used.  
A single stack stored both:

- Program data (operands, arguments, intermediate values)
- Control metadata (return address, previous frame pointer)

This design was intentionally chosen to keep the VM **minimal**, **explicit**, and well-aligned with the educational goals of the assignment.

---

### Why a Unified Stack Was Chosen Initially

The unified stack model offered several advantages:

- Simple implementation with minimal state
- Explicit behavior with no hidden execution state
- Clear detection of stack underflow and overflow errors
- Strong pedagogical value by enforcing strict stack discipline

Under this design, a function call produced the following stack layout:

[arg1 arg2 ... argN | return_pc | old_fp]


Programs were required to explicitly manage arguments and could not assume that user data remained at the top of the stack after a `CALL`.  
This behavior was documented as part of the VM’s **ABI (Application Binary Interface)**, and programs violating this convention were classified as invalid.

---

### Limitation Identified During Testing

During extensive testing—particularly with recursive and control-flow-heavy programs—a fundamental limitation of the unified stack model became apparent:

- Generic stack instructions (`DUP`, `JZ`, `EQ`, etc.) operate on call-frame metadata immediately after a `CALL`
- Recursive programs require manual stack reshuffling or temporary storage
- Code becomes fragile and unintuitive despite being logically correct

Attempts to hide metadata by adjusting the stack pointer introduced semantic inconsistencies such as infinite recursion and incorrect base-case evaluation.  
These experiments demonstrated that the issue was **structural**, not incidental.

---

### Motivation for Transitioning to a Two-Stack Design

To resolve these issues without relying on fragile ABI assumptions or test-case rewrites, the VM architecture was revised to use a **two-stack design**:

- **Data Stack**
  - Stores operands, arguments, intermediate values, and return values
  - All arithmetic, logical, and control instructions operate exclusively on this stack

- **Call Stack**
  - Stores return addresses and frame pointers
  - Accessed only by `CALL` and `RET`
  - Hidden from generic stack instructions

This separation removes ambiguity between program data and control metadata.

---

### Benefits of the Two-Stack Design

- Correct semantics for generic stack operations
- Natural expression of recursion and loops
- Elimination of accidental interaction with call metadata
- Cleaner and more robust calling convention
- Improved compatibility with externally written test cases

This approach closely resembles real execution models such as the JVM (operand stack plus call frames) and modern CPU calling conventions.

---

### Summary

| Design                | Outcome                                 |
|-----------------------|-----------------------------------------|
| Unified Stack         | Minimal, explicit, educational          |
| Identified Limitation | Metadata interferes with stack semantics|
| Attempted Workaround  | Unsafe and semantically incorrect       |
| Two-Stack Design      | Robust, clean, and scalable             |

---

### Design Philosophy

> *The VM prioritizes correctness and clear semantics over heuristic fixes.  
> When a design trade-off was found to limit expressiveness, the architecture was revised rather than masked.*

