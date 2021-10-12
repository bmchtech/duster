#include "inc.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

/**
 * Takes a file pointer and reads the full contents of the file.
 * The caller is responsible for cleaning up the file pointer and buffer.
 */
FileReadResult util_read_file_contents(FILE *fp)
{
    // get the file size
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    // allocate buffer with file size and nullterm
    char *buffer = malloc((size + 1) * sizeof(*buffer));

    fread(buffer, size, 1, fp); // read chunk to buffer
    buffer[size] = '\0';        // add null terminator

    FileReadResult res = {.content = buffer, .size = size};
    return res;
}
