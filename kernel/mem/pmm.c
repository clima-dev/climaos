#include <stivale2.h>
#include <stdint.h>
#include <string/printf.h>
#include <utils/mem.h>
#include "mem/pmm.h"


#define SET_PAGE(page) (phys_bitmap[page / 8] |= (1 << (page % 8)))
#define CLEAR_PAGE(page) (phys_bitmap[page / 8] &= ~(1 << (page % 8)))
#define CHECK_PAGE(page) (phys_bitmap[page / 8] & (1 << (page % 8)))

static size_t lastI = 0;
uint8_t *phys_bitmap = {0};

void phys_alloc_page(void *addr)
{
    SET_PAGE((uint64_t)addr / PAGE_SIZE);
}

void phys_free_page(void *addr)
{
    CLEAR_PAGE((uint64_t)addr / PAGE_SIZE);
}

void phys_alloc_multi(void *addr, uint64_t n)
{
    for (uint64_t i = 0; i < n; i++)
        phys_alloc_page(addr + i * PAGE_SIZE);
}

void phys_free_multi(void *addr, uint64_t n)
{
    for (uint64_t i = 0; i < n; i++)
        phys_free_page(addr + i * PAGE_SIZE);
}

void *phys_alloc(size_t count)
{
    uint64_t free_base = 0;
    uint64_t free_top = 0;

    while(CHECK_PAGE(free_base)) // while theres no free pages
    {
        free_base++;
        free_top++; // just increase the pages till we find one!!! lets go
    }

    while(CHECK_PAGE(free_top) == 0) // while we find free pages
        free_top++;
    
    uint64_t addr = free_base * PAGE_SIZE;
    phys_alloc_page((void *)(addr));
    // we need to round up what we allocated, for example 300 bytes will be 1 page, because the page size is 4096, 5000 bytes will be 2 pages etc.
    free_base += (count + (PAGE_SIZE - 1)) / PAGE_SIZE;
    return (void *)(addr);
}

void init_phys(struct stivale2_struct_tag_memmap *mem_tag)
{
    uint64_t total_memory = 0;
    uint64_t available_memory = 0;
    
    uint32_t bitmap_bytes = 0;

    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        /* code */
        struct stivale2_mmap_entry mem_entry = mem_tag->memmap[i];
        total_memory += mem_entry.length;
        if (mem_entry.type == STIVALE2_MMAP_USABLE)
        {
            printf("[PHYS] usable, from 0x%lx to 0x%lx\n", mem_entry.base, mem_entry.length);
            available_memory += mem_entry.length;
            bitmap_bytes = (mem_entry.base + mem_entry.length) / PAGE_SIZE / 8;
        }
    }

    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        struct stivale2_mmap_entry *mem_entry = &mem_tag->memmap[i];
        if (mem_entry->type == STIVALE2_MMAP_USABLE && mem_entry->length > bitmap_bytes)
        {
            phys_bitmap = (uint8_t *)mem_entry->base;

            uint64_t bitmap_bytes_up = ((bitmap_bytes + (PAGE_SIZE - 1)) / PAGE_SIZE);
            bitmap_bytes_up *= PAGE_SIZE;

            mem_entry->base += bitmap_bytes_up;
            mem_entry->length -= bitmap_bytes_up;
            break;
        }
    }

    memset(phys_bitmap, 0xFF, bitmap_bytes);
    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        struct stivale2_mmap_entry mem_entry = mem_tag->memmap[i];
        if (mem_entry.type == STIVALE2_MMAP_USABLE)
            phys_free_multi((void *)mem_entry.base, mem_entry.length / PAGE_SIZE);
    }

    printf("[PHYS] %uMB available, %uMB total\n", available_memory / (1024 * 1024), total_memory / (1024 * 1024));
}