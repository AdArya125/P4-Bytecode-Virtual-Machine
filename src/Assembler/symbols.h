#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    char *name;
    uint32_t addr;
} Label;

typedef struct
{
    Label *items;
    size_t count;
    size_t cap;
} LabelTable;

void labels_init(LabelTable *t);
void labels_free(LabelTable *t);
void labels_add(LabelTable *t, const char *name, uint32_t addr);
int labels_find(const LabelTable *t, const char *name, uint32_t *out_addr);
void labels_dump(LabelTable *t);
#endif
