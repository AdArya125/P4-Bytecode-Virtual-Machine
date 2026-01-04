#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
// #include "../instruction.h"

static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s input.asm [-o output.bin]\n", prog);
}

static char *default_output_name(const char *input)
{
    static char buf[256];
    snprintf(buf, sizeof(buf), "%s.bin", input);
    return buf;
}
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = default_output_name(input_path);

    if (argc == 4)
    {
        if (strcmp(argv[2], "-o") != 0)
        {
            usage(argv[0]);
            return 1;
        }
        output_path = argv[3];
    }
    else if (argc != 2)
    {
        usage(argv[0]);
        return 1;
    }

    int rc = assemble_file(input_path, output_path);
    if (rc != 0)
    {
        fprintf(stderr, "Assembly failed\n");
    }

    return rc;
}
