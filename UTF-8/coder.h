#pragma once
#include <stdio.h>
#include <inttypes.h>

#define MaxCodeLength 4

typedef struct
{
    uint8_t code[MaxCodeLength];
    uint8_t length;
} CodeUnit;

int encode(const uint32_t code_point, CodeUnit *code_unit);

uint32_t decode(const CodeUnit *code_unit);

int read_next_code_unit(FILE *in, CodeUnit *code_units);

int write_code_unit(FILE *out, const CodeUnit *code_unit);