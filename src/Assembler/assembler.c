#include "assembler.h"
#include "lexer.h"
#include "symbols.h"
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
static size_t instr_size(uint8_t opcode)
{
    switch (opcode)
    {
    case OP_PUSH:
    case OP_STORE:
    case OP_LOAD:
    case OP_JMP:
    case OP_JZ:
    case OP_LOAD_LOCAL:
    case OP_STORE_LOCAL:
        return 1 + 4;

    case OP_CALL:
        return 1 + 4 + 4; // addr + nargs

    default:
        return 1;
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

    LabelTable labels;
    labels_init(&labels);

    char line[512];
    uint32_t pc = 0;
    unsigned lineno = 0;

    // -------- Pass 1: labels + sizes --------
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
            // printf("Found lable at %d\n", lineno);
            // printf("%s\n", p);
            *colon = '\0';
            // printf("%s\n", p);
            char *name = lx_trim(p);
            if (*name == '\0')
            {
                fprintf(stderr, "Assembler: empty label on line %u\n", lineno);
                goto error;
            }

            // printf("%s %d\n", name, pc);
            labels_add(&labels, name, pc);
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
            fprintf(stderr, "Assembler: unknown mnemonic '%s' on line %u\n", mn, lineno);
            goto error;
        }

        pc += instr_size(op);
    }
    if (labels.count)
        labels_dump(&labels);
    // -------- Pass 2: emit code --------

    rewind(in);
    FILE *out = fopen(output_path, "wb");
    if (!out)
    {
        fprintf(stderr, "Assembler: cannot open '%s' for writing\n", output_path);
        goto error;
    }

    pc = 0;
    lineno = 0;

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
        char *arg1 = NULL;
        if (!arg)
        {
            printf("\n");
            continue; /* no operand */
        }

        uint32_t operand = 0;
        int32_t operand1 = 0;
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
        case OP_LOAD_LOCAL:
        case OP_STORE_LOCAL:
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
            if (!labels_find(&labels, arg, &operand))
            {
                fprintf(stderr, "Assembler: unknown label '%s' on line %u\n", arg, lineno);
                goto error_out;
            }
            break;
        }
        case OP_CALL:
        {

            arg1 = strtok(NULL, " \t\r\n");

            if (!labels_find(&labels, arg, &operand))
            {
                fprintf(stderr, "Assembler: unknown label '%s' on line %u\n", arg, lineno);
                goto error_out;
            }

            operand1 = parse_i32(arg1, &ok);
            if (!ok || operand1 < 0)
            {
                // error("invalid nargs");
                fprintf(stderr, "Assembler: invalid nargs for function '%s' on line %u\n", arg, lineno);
                goto error_out;
            }

            if (!arg1)
            {
                fprintf(stderr, "Assembler: missing nargs for function '%s' on line %u\n", arg, lineno);
                goto error_out;
            }
            break;
        }
        case OP_RET:
        {
            if (arg)
            {
                fprintf(stderr,
                        "Assembler: 'RET' takes no operands (line %u)\n",
                        lineno);
                goto error_out;
            }
            break;
        }
        default:
            fprintf(stderr, "Assembler: unexpected operand for '%s' on line %u : %s\n", mn, lineno, arg);
            goto error_out;
        }

        uint8_t b0 = (uint8_t)(operand & 0xFF);
        uint8_t b1 = (uint8_t)((operand >> 8) & 0xFF);
        uint8_t b2 = (uint8_t)((operand >> 16) & 0xFF);
        uint8_t b3 = (uint8_t)((operand >> 24) & 0xFF);
        if (op != OP_CALL)
        {
            printf("%u %u %u %u\n", (unsigned)b0, (unsigned)b1, (unsigned)b2, (unsigned)b3);
            fputc(b0, out);
            fputc(b1, out);
            fputc(b2, out);
            fputc(b3, out);
            pc += 4;
        }
        else
        {
            printf("%u %u %u %u ", (unsigned)b0, (unsigned)b1, (unsigned)b2, (unsigned)b3);
            fputc(b0, out);
            fputc(b1, out);
            fputc(b2, out);
            fputc(b3, out);
            b0 = (uint8_t)(operand1 & 0xFF);
            b1 = (uint8_t)((operand1 >> 8) & 0xFF);
            b2 = (uint8_t)((operand1 >> 16) & 0xFF);
            b3 = (uint8_t)((operand1 >> 24) & 0xFF);
            printf("| %u %u %u %u\n", (unsigned)b0, (unsigned)b1, (unsigned)b2, (unsigned)b3);
            fputc(b0, out);
            fputc(b1, out);
            fputc(b2, out);
            fputc(b3, out);
            pc += 8;
        }
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
