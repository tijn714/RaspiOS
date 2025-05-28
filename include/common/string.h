#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdbool.h>

#include "common/types.h"

// strlen
size_t strlen(const char* str);

// itoa
char* itoa(int value, char* str, int base);

//memset
void* memset(void* ptr, int value, size_t num);

//memcpy
void* memcpy(void* dest, const void* src, size_t num);

// strstr
char* strstr(const char* haystack, const char* needle);

// strncmp
int strncmp(const char* str1, const char* str2, size_t num);

// strcmp
int strcmp(const char* str1, const char* str2);

// is functions
bool isint(char c);

int isspace(char c);

int isalpha(char c);
char upper(char c);
char lower(char c);

void clear_buffer(char* buffer);

#endif // STRING_H 