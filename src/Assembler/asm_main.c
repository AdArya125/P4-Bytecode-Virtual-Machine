#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "../instruction.h"
static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s input.asm -o output.bin\n", prog);
}

int main(int argc, char **argv)
{
    if (argc != 4 || strcmp(argv[2], "-o") != 0)
    {
        usage(argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[3];

    int rc = assemble_file(input_path, output_path);
    if (rc != 0)
    {
        fprintf(stderr, "Assembly failed\n");
    }
    return rc;
}
