#include "lexer.h"
#include "../instruction.h"
#include <string.h>
#include <ctype.h>

int lx_read_line(FILE *f, char *buf, size_t sz)
{
    if (!fgets(buf, (int)sz, f))
        return 0;
    return 1;
}

char *lx_trim(char *s)
{
    while (*s && isspace((unsigned char)*s))
        s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1]))
        --end;
    *end = '\0';
    return s;
}

void lx_chomp_comment(char *s)
{
    char *semi = strchr(s, ';');
    if (semi)
    {
        *semi = '\0'; /* truncate line at ';' */
    }
}

int lx_is_blank_or_comment(const char *s)
{
    while (*s && isspace((unsigned char)*s))
        s++;
    return *s == '\0' || *s == ';';
}

int lx_lookup_opcode(const char *mnemonic, uint8_t *out)
{
    if (strcmp(mnemonic, "PUSH") == 0)
    {
        *out = OP_PUSH;
        return 1;
    }
    if (strcmp(mnemonic, "DUP") == 0)
    {
        *out = OP_DUP;
        return 1;
    }
    if (strcmp(mnemonic, "SWAP") == 0)
    {
        *out = OP_SWAP;
        return 1;
    }
    if (strcmp(mnemonic, "DROP") == 0)
    {
        *out = OP_DROP;
        return 1;
    }
    if (strcmp(mnemonic, "OVER") == 0)
    {
        *out = OP_OVER;
        return 1;
    }

    if (strcmp(mnemonic, "ADD") == 0)
    {
        *out = OP_ADD;
        return 1;
    }
    if (strcmp(mnemonic, "SUB") == 0)
    {
        *out = OP_SUB;
        return 1;
    }
    if (strcmp(mnemonic, "MUL") == 0)
    {
        *out = OP_MUL;
        return 1;
    }
    if (strcmp(mnemonic, "DIV") == 0)
    {
        *out = OP_DIV;
        return 1;
    }

    if (strcmp(mnemonic, "INC") == 0)
    {
        *out = OP_INC;
        return 1;
    }
    if (strcmp(mnemonic, "DEC") == 0)
    {
        *out = OP_DEC;
        return 1;
    }
    if (strcmp(mnemonic, "NEG") == 0)
    {
        *out = OP_NEG;
        return 1;
    }

    if (strcmp(mnemonic, "EQ") == 0)
    {
        *out = OP_EQ;
        return 1;
    }
    if (strcmp(mnemonic, "LT") == 0)
    {
        *out = OP_LT;
        return 1;
    }
    if (strcmp(mnemonic, "GT") == 0)
    {
        *out = OP_GT;
        return 1;
    }

    if (strcmp(mnemonic, "PRINT") == 0)
    {
        *out = OP_PRINT;
        return 1;
    }
    if (strcmp(mnemonic, "NOP") == 0)
    {
        *out = OP_NOP;
        return 1;
    }

    if (strcmp(mnemonic, "JMP") == 0)
    {
        *out = OP_JMP;
        return 1;
    }
    if (strcmp(mnemonic, "JZ") == 0)
    {
        *out = OP_JZ;
        return 1;
    }

    if (strcmp(mnemonic, "STORE") == 0)
    {
        *out = OP_STORE;
        return 1;
    }
    if (strcmp(mnemonic, "LOAD") == 0)
    {
        *out = OP_LOAD;
        return 1;
    }

    if (strcmp(mnemonic, "HALT") == 0)
    {
        *out = OP_HALT;
        return 1;
    }
    if (strcmp(mnemonic, "CALL") == 0)
    {
        *out = OP_CALL;
        return 1;
    }
    if (strcmp(mnemonic, "RET") == 0)
    {
        *out = OP_RET;
        return 1;
    }

    return 0;
}
