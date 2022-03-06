#pragma once
#include <stdint.h>

typedef struct {
    uint16_t offset_1;        // offset bits 0..15
    uint16_t selector;        // a code segment selector in GDT or LDT
    uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;        // offset bits 16..31
    uint32_t offset_3;        // offset bits 32..63
    uint32_t zero;            // reserved
} idt_entry_t;

typedef struct
{ /* lidt instruction will load this into the IDTR(egister) */
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) idtr_t;

void idt_set_entry(uint16_t n, uint64_t handler);
void init_idt();