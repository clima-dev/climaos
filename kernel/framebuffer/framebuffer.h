#pragma once
#include <stivale2.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>


void init_fb();
void print(const char* string);
size_t strlen(const char* string);
void printc(char c);