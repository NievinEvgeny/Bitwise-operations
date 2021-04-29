#include <inttypes.h>
#include "command.h"
#include "coder.h"
#include <stdio.h>

int decode_file(const char *in_file_name, const char *out_file_name)
{
    CodeUnit code_unit;
    FILE *in = fopen(in_file_name, "r");
    FILE *out = fopen(out_file_name, "w+b");

    if ((in == NULL) || (out == NULL))
    {
        printf("Ошибка открытия файла\n");
        return -1;
    }

    while (!feof(in))
    {
        if (read_next_code_unit(in, &code_unit) == -1)
        {
            break;
        }

        uint32_t buf = decode(&code_unit);

        fprintf(out, "%" PRIx32 "\n", buf);
    }
    fclose(in);
    fclose(out);
    return 0;
}

int encode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "r");
    FILE *out = fopen(out_file_name, "w+b");

    if ((in == NULL) || (out == NULL))
    {
        printf("Ошибка открытия файла\n");
        return -1;
    }

    while (!feof(in))
    {
        CodeUnit code_unit;
        uint32_t code_point;

        fscanf(in, "%" SCNx32, &code_point);

        if (encode(code_point, &code_unit) == -1)
        {
            continue;
        }

        if (write_code_unit(out, &code_unit) == -1)
        {
            continue;
        }
    }
    fclose(in);
    fclose(out);
    return 0;
}