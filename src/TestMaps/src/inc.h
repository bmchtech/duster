#pragma once

// #include "util/cute_tiled.h"
#include <stdio.h>

typedef struct
{
    char *content;
    size_t size;
} FileReadResult;

FileReadResult util_read_file_contents(FILE *fp);