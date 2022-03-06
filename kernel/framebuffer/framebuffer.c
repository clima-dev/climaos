#include <framebuffer/framebuffer.h>
#include <kernel.h>

void (*write)(const char*, uint64_t);

void init_fb()
{
    write = (void (*)(const char *, uint64_t))(term_tag->term_write);
}

size_t _strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 

void print(const char* string)
{
    write(string, _strlen(string));
}

void printc(char c)
{
    char* string;
    string[0] = c;
    print(string);
}