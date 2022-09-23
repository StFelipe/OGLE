#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

char* ReadFile(const char* path)
{
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Couldn't open file: %s", path);
        return NULL;
    }

    char* buffer = NULL;
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    buffer = malloc(length + 1); // fread doesn't \0 terminate
    
    if (buffer) {
        fseek(file, 0, SEEK_SET);
        fread(buffer, 1, length, file);
        buffer[length] = '\0';
    }

    fclose(file);
    return buffer;
}

#endif