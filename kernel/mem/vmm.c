#include "mem/vmm.h"

uint64_t *kernel_page_map = 0;

void virt_load_page_map(uint64_t pml4)
{
    /* in long mode, the CR3 register is used to point to the PML4 base address */
    asm("mov %0, %%cr3" ::"a"(pml4));
}   

void virt_invlpg(uint64_t virt_addr)
{
    /* TLB stands for translation lookaside buffer */
    asm("invlpg (%0)" ::"r"(virt_addr)); /* invlpg flushes/invalidates the specified TLB entry */
}

uint64_t *virt_get_table(uint64_t *table, uint16_t offset, uint64_t flags) {
    if(!(table[offset] & 0x01)) {
        table[offset] = (uint64_t)phys_alloc(PAGE_SIZE);
        table[offset] |= flags;
    }

    return (uint64_t)((table[offset] & ~0xFFF));
}

void virt_map_page(uint64_t* page_map, uint64_t phys_addr, uint64_t virt_addr, uint64_t flags)
{
    uint16_t pml4_offset = (virt_addr & ((uint64_t)0x1ff << 39)) >> 39;
    uint16_t pdp_offset = (virt_addr & ((uint64_t)0x1ff << 30)) >> 30;
    uint16_t pd_offset = (virt_addr & ((uint64_t)0x1ff << 21)) >> 21;
    uint16_t pt_offset = (virt_addr & ((uint64_t)0x1ff << 12)) >> 12;

    uint64_t *pdp = virt_get_table(page_map, pml4_offset, flags);
    uint64_t *pd = virt_get_table(pdp, pdp_offset, flags);
    uint64_t *pt = virt_get_table(pd, pd_offset, flags);

    pt[pt_offset] = phys_addr | flags;
    virt_invlpg(virt_addr); /* flush the TLB */
}

void virt_map_multi(uint64_t *page_map, uint64_t base, uint64_t end, uint64_t offset, uint64_t flags)
{
    for (uint64_t i = base; i < end; i += PAGE_SIZE)
        virt_map_page(page_map, i, i + offset, flags);
}

void init_virt()
{
    kernel_page_map = (uint64_t*)phys_alloc(PAGE_SIZE);
    memset((void *)kernel_page_map, 0, PAGE_SIZE);
    printf("[VIRT] page_map = 0x%x, remapping memory: ", kernel_page_map);

    printf("%d\n", kernel_page_map);

    virt_map_multi(kernel_page_map, 0, 0x8000000, 0, 0b11);
    printf("[id map]");
    virt_map_multi(kernel_page_map, 0, 0x100000000, HIGHER_HALF, 0b11);

    // Gotta map the kernel too!
    virt_map_multi(kernel_page_map, 0, 0x2000000, KERNEL_OFFS, 0b11);
    printf("[data]");

    virt_load_page_map((uint64_t)kernel_page_map);

    printf("[VIRT] Done pog!\n");
}