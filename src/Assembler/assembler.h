#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/* Assemble input_path (.asm) to output_path (.bin). 0 = OK, non-zero = error. */
int assemble_file(const char *input_path, const char *output_path);

#endif
