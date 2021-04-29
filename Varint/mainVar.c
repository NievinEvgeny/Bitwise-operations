#include "Varint.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX 1000000

int main()
{
    uint32_t number;
    uint8_t buf[4];
    uint8_t *buffer;
    int size_of_encode, size_of_buffer;
    uint32_t decoded_number;
    const uint8_t **bufp;
    int i;
    FILE *Uncompressed_file = fopen("uncompressed.dat", "w+b");
    FILE *Compressed_file = fopen("compressed.dat", "w+b");

    if ((Uncompressed_file == NULL) || (Compressed_file == NULL))
    {
        printf("Ошибка открытия файла\n");
        return -1;
    }

    for (int i = 0; i < MAX; i++)
    {
        number = generate_number();
        fwrite(&number, sizeof(uint32_t), 1, Uncompressed_file);
        size_of_encode = encode_varint(number, buf);
        fwrite(buf, sizeof(uint8_t), size_of_encode, Compressed_file);
    }

    rewind(Uncompressed_file);
    bufp = (const uint8_t **)&buffer;
    fseek(Compressed_file, 0, SEEK_END);
    size_of_buffer = ftell(Compressed_file);
    rewind(Compressed_file);
    buffer = (uint8_t *)malloc(size_of_buffer);
    fread(buffer, sizeof(uint8_t), size_of_buffer, Compressed_file);

    for (i = 0; i < MAX; i++)
    {
        decoded_number = decode_varint(bufp);
        fread(&number, sizeof(uint32_t), 1, Uncompressed_file);

        if (number != decoded_number)
        {
            printf("Ошибка: числа не совпадают\n");
            break;
        }
    }
    return 0;
}