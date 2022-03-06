#include <stddef.h>

#include <framebuffer/framebuffer.h>
#include <framebuffer/tamsyn8x16.h>

uint32_t *fb;
uint16_t fb_height, fb_width;

static uint32_t fg_color = DEFAULT_FG, bg_color = DEFAULT_BG;

int print_x = 0, print_y = 0;

void init_fb(struct stivale2_struct_tag_framebuffer *fb_tag)
{
    fb = (uint32_t *)fb_tag->framebuffer_addr;
    fb_width = fb_tag->framebuffer_width;
    fb_height = fb_tag->framebuffer_height;

    for (size_t i = 0; i < (fb_width * fb_height); i++)
        fb[i] = bg_color; /* fills the entire background for the first time */
}

void scroll()
{
    /* moves every pixel so we can actually scroll */
    for (size_t i = 0; i < (fb_width * fb_height); i++)
        fb[i] = (fb + fb_width * FONT_HEIGHT)[i];

    /* fills the new line with the background color (because it is empty!) */
    for (size_t i = (fb_width * (fb_height - FONT_HEIGHT)); i < (fb_width * fb_height); i++)
        fb[i] = bg_color;
}

void set_pixel(int x, int y, uint32_t color)
{
    /* plot a pixel in the framebuffer */
    fb[y * fb_width + x] = color;
}

void put_literal_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color)
{
    for (int ly = 0; ly < FONT_HEIGHT; ly++)
    {
        for (int lx = 0; lx < FONT_WIDTH; lx++)
        {
            uint8_t pixel = tamsyn8x16[(c * FONT_HEIGHT) + ly];
            if ((pixel >> lx) & 1)
                fb[x + ((FONT_WIDTH - 1) - lx) + ((y + ly) * fb_width)] = fg_color;
            else
                fb[x + ((FONT_WIDTH - 1) - lx) + ((y + ly) * fb_width)] = bg_color;
        }
    }
}

void set_color(uint32_t fg, uint32_t bg)
{
    fg_color = fg, bg_color = bg;
}

void putc(char c)
{
    switch (c)
    {
    case '\n': /* new line */
        print_x = 0;
        if (print_y + FONT_HEIGHT * 2 > fb_height)
            scroll();
        else
            print_y += FONT_HEIGHT;
        break;
    case '\r': /* carriage return */
        print_x = 0;
        break;
    case '\b': /* back (del) */
        print_x -= FONT_WIDTH;
        putc(' ');
        print_x -= FONT_WIDTH;
        break;
    default: /* any other character */
        if (print_x + FONT_WIDTH > fb_width)
        {
            print_x = 0;
            print_y += FONT_HEIGHT;
        }
        if (print_y + FONT_HEIGHT > fb_height)
        {
            scroll();
            print_y -= FONT_HEIGHT;
        }
        put_literal_char(print_x, print_y, c, fg_color, bg_color);
        print_x += FONT_WIDTH;
    }
}

void print(const char *s)
{
    for (size_t i = 0; s[i]; i++)
        putc(s[i]);
}