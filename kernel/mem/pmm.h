#pragma once

#include <stivale2.h>

extern uint8_t *phys_bitmap;

void* phys_malloc(size_t count);
void init_phys(struct stivale2_struct_tag_memmap *mem_tag);
