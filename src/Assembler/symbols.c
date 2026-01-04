#include "symbols.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------
 * Initialize label table
 * ------------------------------------------------------------ */
void labels_init(LabelTable *t)
{
    t->items = NULL;
    t->count = 0;
    t->cap = 0;
}

/* ------------------------------------------------------------
 * Free label table and reset state
 * ------------------------------------------------------------ */
void labels_free(LabelTable *t)
{
    for (size_t i = 0; i < t->count; ++i)
        free(t->items[i].name);

    free(t->items);

    t->items = NULL;
    t->count = 0;
    t->cap = 0;
}

/* ------------------------------------------------------------
 * Local strdup implementation (POSIX-like)
 * ------------------------------------------------------------ */
char *my_strdup(const char *s)
{
    if (s == NULL)
        return NULL;

    size_t len = strlen(s) + 1;

    char *copy = malloc(len);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);
    return copy;
}

/* ------------------------------------------------------------
 * Add a label to the table
 * ------------------------------------------------------------ */
void labels_add(LabelTable *t, const char *name, uint32_t addr)
{
    if (!name)
    {
        fprintf(stderr, "Assembler: NULL label name\n");
        exit(1);
    }

    if (t->count == t->cap)
    {
        size_t ncap = t->cap ? t->cap * 2 : 16;
        Label *n = realloc(t->items, ncap * sizeof *n);
        if (!n)
        {
            fprintf(stderr, "Assembler: out of memory in labels_add\n");
            exit(1);
        }
        t->items = n;
        t->cap = ncap;
    }

    t->items[t->count].name = my_strdup(name);
    if (!t->items[t->count].name)
    {
        fprintf(stderr, "Assembler: out of memory copying label name\n");
        exit(1);
    }

    t->items[t->count].addr = addr;
    t->count++;
}

/* ------------------------------------------------------------
 * Find label by name
 * ------------------------------------------------------------ */
int labels_find(const LabelTable *t, const char *name, uint32_t *out_addr)
{
    if (!name)
        return 0;

    for (size_t i = 0; i < t->count; ++i)
    {
        if (strcmp(t->items[i].name, name) == 0)
        {
            if (out_addr)
                *out_addr = t->items[i].addr;
            return 1;
        }
    }
    return 0;
}

/* ------------------------------------------------------------
 * Debug dump of label table
 * ------------------------------------------------------------ */
void labels_dump(LabelTable *t)
{
    printf("DEBUG: Label Table:\n");
    for (size_t i = 0; i < t->count; ++i)
    {
        printf("%zu : %s | %u\n",
               i + 1,
               t->items[i].name,
               (unsigned)t->items[i].addr);
    }
    printf("____________________\n");
}
