#pragma once

#include <stdint.h>

void *memset(void *buffptr, int value, size_t size) {
    uint8_t *buff = (uint8_t *)buffptr;
    for (size_t i = 0; i < size; i++)
        buff[i] = (uint8_t)value;
    return buffptr;
}
