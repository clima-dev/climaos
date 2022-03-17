#pragma once

#include <stivale2.h>

#define PAGE_SIZE 4096

extern uint8_t *phys_bitmap;

void* phys_alloc(size_t count);
void phys_free_page(void* addr);
void init_phys(struct stivale2_struct_tag_memmap *mem_tag);
