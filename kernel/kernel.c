#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>
#include <isr.h>
#include <string/printf.h>
#include <framebuffer/framebuffer.h>
#include <mem/pmm.h>

static uint8_t stack[8192];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    // All tags need to begin with an identifier and a pointer to the next tag.
    .tag = {
        // Identification constant defined in stivale2.h and the specification.
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        // If next is 0, it marks the end of the linked list of header tags.
        .next = 0
    },
    // The terminal header tag possesses a flags field, leave it as 0 for now
    // as it is unused.
    .flags = 0
};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // Same as above.
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) {
            return NULL;
        }

        if (current_tag->identifier == id) {
            return current_tag;
        }

        current_tag = (void *)current_tag->next;
    }
}
struct stivale2_struct_tag_framebuffer *frm_tag;
struct stivale2_struct_tag_terminal *term_tag;

void alloc_test() {
    int* ptr1;
    ptr1 = (int*)phys_alloc(3 * sizeof(int));

    if(ptr1==NULL){
        printf("Memory not allocated. \n");
    }
    else{
        printf("Memory allocated succesfully. \n");
    };
    printf("The address of the pointer is:%u\n ", ptr1);
};

// The following will be our kernel's entry point.
void _start(struct stivale2_struct *stivale2_struct) {
    frm_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    term_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);

    init_fb();
    printf("[OK] %s\n", "FB");

    init_isr();
    printf("[OK] ISR\n");

    init_phys(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    printf("[OK] PHYS\n");

    printf("END OF BOOT\nWELCOME TO CLIMOS");
    alloc_test(); // sketchy form of allocating memory, wrote it in like 5s because i wanted a quick test of it. ALLOCATING WORKS IS THAT A POG????
    for (;;) {
        asm ("hlt");
    };
};
