#pragma once
#include <stdio.h>
#include <stdlib.h>
inline void errif(bool condition, const char *error_message)
{
    if (condition)
    {
        perror(error_message);
        exit(EXIT_FAILURE);
    }
}