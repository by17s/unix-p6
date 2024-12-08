#ifndef __TOOLS_H_
#define __TOOLS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <debug.h>

#include <types.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UINT8_MAX)
#define HIGHS (ONES * (UINT8_MAX/2+1))
#define HASZERO(X) (((X)-ONES) & ~(X) & HIGHS)
#define BIT_SET(BITMAP, BIT) (BITMAP[(BIT) / 8] |= (1 << ((BIT) % 8)))
#define BIT_CLEAR(BITMAP, BIT) (BITMAP[(BIT) / 8] &= ~(1 << ((BIT) % 8)))
#define BIT_GET(BITMAP, BIT) ((BITMAP[(BIT) / 8] >> ((BIT) % 8)) & 1)
#define BITOP(A, B, OP) \
 ((A)[(size_t)(B)/(8*sizeof *(A))] OP (size_t)1<<((size_t)(B)%(8*sizeof *(A))))


typedef int bool;

#define true 1
#define false 0

typedef int cmd_t;

void tmemcpy(void *dest, void *src, uint32_t n);

void tmemcpyw(void *dest, void *src, uint32_t n);

void tmemcpyd(void *dest, void *src, uint32_t n);

void tmemcpyd_inv(void *arr, uint32_t n);

int tmemcmp(const void *s1, const void *s2, size_t n);

void *tmemset(void *ptr, uint8_t n, uint32_t size);

void *tmemsetw(void *ptr, uint16_t n, uint32_t size);

void *tmemsetd(void *ptr, uint32_t n, uint32_t size);

void * memchr(const void * src, int c, size_t n);

void tfastcpy(unsigned char *dst, unsigned char *src, size_t s);

uint32_t strlen(const char *str);

char *strcpy(char *dest, const char *src);

int strcmp(const char *str1, const char *str2);

int strncmp(const char *s1, const char *s2, size_t n);

const char* strrchr(const char* str, int target);

char * strchr(const char * s, int c);

size_t lfind(const char * str, const char accept);

size_t rfind(const char * str, const char accept);

char* strtok_r(char * str, const char * delim, char ** saveptr);

char* strtok(char * str, const char * delim);

char* strcat(char *dest, const char *src);

char* strncat(char *dest, const char *src, size_t n);

char* strstr(const char * h, const char * n);

uint8_t startswith(const char * str, const char * accept);

char* strdup(const char* s);

int findch(uint32_t start_index, char target, char *str);

int str_to_upper(int c);

int str_to_lower(int c);

uint32_t str_to_uint(char *str);

void reverse_str(char *str);

bool check_eq_of_strings(char *s1, char *s2);

// Функция взята из https://github.com/0Nera/BMOSP
// Преобразование целого числа "i" в системе счисления "base" в строку "buf"
void int_to_str(int32_t i, uint8_t base, char *buf);

// Функция взята из https://github.com/0Nera/BMOSP
// Преобразование неотрицательного целого числа "i" в системе счисления "base" в строку "buf"
void uint_to_str(uint32_t i, uint8_t base, char *buf);

uint16_t tflip_uint16(uint16_t num);

void __doprnt_put_string(char* str, void (*_putch)(char));

void __doprnt(const char* fmt, va_list argp, void (*_putch)(char));

void _cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#endif //__TOOLS_H_