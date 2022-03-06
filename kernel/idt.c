#include <stdint.h>
#include <idt.h>

/* remember that IDT stands for Interrupt Descriptor Table
   the CPU will use this to know what to do when an interrupt is fired */

__attribute__((aligned(0x10))) /* alignment is a matter of performance */
static idt_entry_t idt[256];   /* define a new IDT with 256 gates/entries */
static idtr_t idtr;

/*
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
 */
void idt_set_entry(uint16_t n, uint64_t handler)
{
    idt[n].offset_1 = (uint16_t)handler;           // 0..15 so its an int 16 of the handler??? probably
    idt[n].selector = 0x28;                        // eee gave the hex to me lmfao, i dont have a clue
    idt[n].ist = 0;                              // i dont know what the hell is an ist
    idt[n].type_attributes = 0x8E;                    // i have no clue  WHAT THAT IS!!!!!
    idt[n].offset_2 = (uint16_t)(handler >> 16);    // haha ez code lmfao get rekt
    idt[n].offset_3 = (uint32_t)(handler >> 32);   // love this osdev
    idt[n].zero = 0;                               // yeah guess why
};

void init_idt()
{
    idtr.base = (uint64_t)&idt;
    idtr.size = 256 * sizeof(idt_entry_t) - 1;
    asm("lidt %0"
        :
        : "m"(idtr));
};