#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>
#include <isr.h>

static uint8_t stack[8192];

void (*term_write)(const char *string, size_t length);

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

extern size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

extern void kprintf(const char* str)
{
    term_write(str, strlen(str));
}

// The following will be our kernel's entry point.
void _start(struct stivale2_struct *stivale2_struct) {
    struct stivale2_struct_tag_terminal *term_str_tag;
    term_str_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);

    if (term_str_tag == NULL) {
        for (;;) {
            asm ("hlt");
        }
    }

    term_write = (void (*)(const char*, size_t))term_str_tag->term_write;

    init_isr();
    kprintf("hold up chat");
    
    kprintf("poggggg");

    for (;;) {
        asm ("hlt");
    }
}
