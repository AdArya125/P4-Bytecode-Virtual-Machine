#include "assembler.h"
#include "lexer.h"
#include "../instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>

/* parse numeric operands */
static int32_t parse_i32(const char *s, int *ok)
{
    char *end = NULL;
    long v = strtol(s, &end, 0);
    if (end == s || *end != '\0' || v < INT32_MIN || v > INT32_MAX)
    {
        *ok = 0;
        return 0;
    }
    *ok = 1;
    return (int32_t)v;
}

static uint32_t parse_u32(const char *s, int *ok)
{
    char *end = NULL;
    unsigned long v = strtoul(s, &end, 0);
    if (end == s || *end != '\0' || v > UINT32_MAX)
    {
        *ok = 0;
        return 0;
    }
    *ok = 1;
    return (uint32_t)v;
}

/* instruction size in bytes for PC accounting */
static uint32_t instr_size(uint8_t opcode)
{
    switch (opcode)
    {
    case OP_PUSH:
    case OP_STORE:
    case OP_LOAD:
    case OP_JMP:
    case OP_JZ:
        return 1 + 4; // 1 byte for instruction and 4 bytes (32 bits) for value
    default:
        return 1; // 1 byte for instruction
    }
}

int assemble_file(const char *input_path, const char *output_path)
{
    FILE *in = fopen(input_path, "r");
    if (!in)
    {
        fprintf(stderr, "Assembler: cannot open '%s'\n", input_path);
        return 1;
    }

    char line[512];
    uint32_t pc = 0;
    unsigned lineno = 0;

    // Need to run one pass to read and store all labels and their positions

    // This is the code of the second pass:

    FILE *out = fopen(output_path, "wb");
    if (!out)
    {
        fprintf(stderr, "Assembler: cannot open '%s' for writing\n", output_path);
        goto error;
    }

    while (lx_read_line(in, line, sizeof(line)))
    {
        lineno++;
        char *p = lx_trim(line);
        lx_chomp_comment(p); /* kill trailing comment */
        if (lx_is_blank_or_comment(p))
            continue;

        char *colon = strchr(p, ':');
        if (colon)
        {
            *colon = '\0';
            p = lx_trim(colon + 1);
            if (*p == '\0' || *p == ';')
            {
                continue;
            }
        }

        char *mn = strtok(p, " \t\r\n");
        if (!mn)
            continue;

        uint8_t op;
        if (!lx_lookup_opcode(mn, &op))
        {
            fprintf(stderr, "Assembler: unknown mnemonic '%s' on line %u (2nd pass)\n", mn, lineno);
            return 1;
        }

        printf("0x%02X ", (unsigned)op);

        fputc(op, out);
        pc++;

        char *arg = strtok(NULL, " \t\r\n");
        if (!arg)
        {
            printf("\n");
            continue; /* no operand */
        }

        uint32_t operand = 0;
        int ok = 1;

        switch (op)
        {
        case OP_PUSH:
        {
            int32_t v = parse_i32(arg, &ok);
            if (!ok)
            {
                fprintf(stderr, "Assembler: invalid PUSH operand '%s' on line %u\n", arg, lineno);
                goto error_out;
            }
            operand = (uint32_t)v;
            break;
        }
        case OP_STORE:
        case OP_LOAD:
        {
            operand = parse_u32(arg, &ok);
            if (!ok)
            {
                fprintf(stderr, "Assembler: invalid index '%s' on line %u\n", arg, lineno);
                goto error_out;
            }
            break;
        }
        case OP_JMP:
        case OP_JZ:
        {
            // left to implement later
            break;
        }
        default:
            fprintf(stderr, "Assembler: unexpected operand for '%s' on line %u\n", mn, lineno);
            goto error_out;
        }

        uint8_t b0 = (uint8_t)(operand & 0xFF);
        uint8_t b1 = (uint8_t)((operand >> 8) & 0xFF);
        uint8_t b2 = (uint8_t)((operand >> 16) & 0xFF);
        uint8_t b3 = (uint8_t)((operand >> 24) & 0xFF);
        printf("%u %u %u %u\n", (unsigned)b0, (unsigned)b1, (unsigned)b2, (unsigned)b3);
        fputc(b0, out);
        fputc(b1, out);
        fputc(b2, out);
        fputc(b3, out);
        pc += 4;
    }

    fclose(out);
    fclose(in);
    return 0;

error_out:
    if (out)
        fclose(out);
error:
    fclose(in);
    return 1;
}
