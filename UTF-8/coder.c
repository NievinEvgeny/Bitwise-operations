#include "coder.h"
#include <inttypes.h>
#include <stdio.h>

int encode(uint32_t code_point, CodeUnit *code_units)
{
    if (code_point <= 0x7f)
    {
        code_units->length = 1;
        code_units->code[0] = code_point;
        return 0;
    }

    if (code_point <= 0x7ff)
    {
        code_units->length = 2;
        code_units->code[1] = 0x80 | (code_point & 0x3F);
        code_units->code[0] = 0xC0 | (code_point >> 6);
        return 0;
    }

    if (code_point <= 0xffff)
    {
        code_units->length = 3;
        code_units->code[2] = 0x80 | (code_point & 0x3F);
        code_units->code[1] = 0x80 | ((code_point & 0xFC0) >> 6);
        code_units->code[0] = 0xE0 | (code_point >> 12);
        return 0;
    }

    if (code_point <= 0x1FFFFF)
    {
        code_units->length = 4;
        code_units->code[3] = 0x80 | (code_point & 0x3F);
        code_units->code[2] = 0x80 | ((code_point & 0xFC0) >> 6);
        code_units->code[1] = 0x80 | ((code_point & 0x3F000) >> 12);
        code_units->code[0] = 0xF0 | (code_point >> 18);
        return 0;
    }
    return -1;
}

uint32_t decode(const CodeUnit *code_unit)
{
    if ((code_unit->code[0] >> 7) == 0)
    {
        return (code_unit->code[0]);
    }

    if (code_unit->code[0] <= 0xDF)
    {
        return ((code_unit->code[1] & 0x3F) | ((code_unit->code[0] & 0x1F) << 6));
    }

    if (code_unit->code[0] <= 0xEF)
    {
        return ((code_unit->code[2] & 0x3F) | ((code_unit->code[1] & 0x3F) << 6) | ((code_unit->code[0] & 0xF) << 12));
    }

    if (code_unit->code[0] <= 0xF7)
    {
        return ((code_unit->code[3] & 0x3F) | ((code_unit->code[2] & 0x3F) << 6) | ((code_unit->code[1] & 0x3F) << 12) | ((code_unit->code[0] & 0x7) << 18));
    }
    return 0;
}

int read_next_code_unit(FILE *in, CodeUnit *code_units)
{
    uint8_t number = 0;

    fread(&number, 1, 1, in);

    while (!feof(in))
    {
        CodeUnit temp;
        temp.length = 0;

        while (number & (1 << (7 - temp.length)))
        {
            temp.length = temp.length + 1;
        }

        if (temp.length > MaxCodeLength)
        {
            fread(&number, 1, 1, in);
            continue;
        }

        if (temp.length == 1)
        {
            fread(&number, 1, 1, in);
            continue;
        }

        if (temp.length == 0)
        {
            temp.length = 1;
        }

        code_units->length = 0;

        for (int i = 1; i <= temp.length; i++)
        {
            temp.code[i - 1] = number;
            code_units->length++;

            if (i == temp.length)
            {
                for (int i = 0; i < code_units->length; i++)
                {
                    code_units->code[i] = temp.code[i];
                }
                return 0;
            }

            fread(&number, 1, 1, in);

            if ((number & 0xC0) != 0x80)
            {
                break;
            }
        }
    }
    return -1;
}

int write_code_unit(FILE *out, const CodeUnit *code_unit)
{
    size_t number_of_bytes = fwrite(code_unit->code, 1, code_unit->length, out);

    if (number_of_bytes == code_unit->length)
    {
        return 0;
    }
    return -1;
}