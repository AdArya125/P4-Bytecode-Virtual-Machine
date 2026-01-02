// temporary code to generate binary fies, won't be needed after assembler is complete

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint8_t program[] =
        {
            0x01, 5, 0, 0, 0, // PUSH 5
            0x18,             // NEG        -> -5
            0x16,             // INC        -> -4
            0x17,             // DEC        -> -5

            0x01, 5, 0, 0, 0, // PUSH 5
            0x19,             // EQ         -> 0

            0x01, 3, 0, 0, 0, // PUSH 3
            0x1A,             // LT         -> 1 (0 < 3? careful!)

            0x60, // PRINT      -> prints result
            0x00, // NOP
            0xFF};

    FILE *f = fopen("../tests/prog6.txt", "wb");
    fwrite(program, 1, sizeof(program), f);
    fclose(f);
    return 0;
}
