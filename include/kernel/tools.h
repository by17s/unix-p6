#ifndef __TOOLS_H_
#define __TOOLS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define BIT_SET(BITMAP, BIT) (BITMAP[(BIT) / 8] |= (1 << ((BIT) % 8)))
#define BIT_CLEAR(BITMAP, BIT) (BITMAP[(BIT) / 8] &= ~(1 << ((BIT) % 8)))
#define BIT_GET(BITMAP, BIT) ((BITMAP[(BIT) / 8] >> ((BIT) % 8)) & 1)

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

void tfastcpy(unsigned char *dst, unsigned char *src, size_t s);

uint32_t strlen(const char *str);

char *strcpy(char *dest, const char *src);

int strcmp(char *str1, const char *str2);

int strncmp(char *s1, char *s2, size_t n);

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