#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>
#include <stdio.h>

/* Reads a line into buf, returns 1 on success, 0 on EOF. */
int lx_read_line(FILE *f, char *buf, size_t sz);

/* Trims whitespace both ends in-place and returns pointer to first non-space. */
char *lx_trim(char *s);

/* Trims trailing comment ';' to end-of-line, in-place. */
void lx_chomp_comment(char *s);

/* Returns 1 if line is empty or comment (starting with ';'), else 0. */
int lx_is_blank_or_comment(const char *s);

/* Map mnemonic string -> opcode (from instruction.h). Returns 1 on success. */
int lx_lookup_opcode(const char *mnemonic, uint8_t *out_opcode);

#endif
