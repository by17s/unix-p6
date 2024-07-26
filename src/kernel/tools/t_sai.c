#include <tools.h>

uint32_t strlen(const char *str)
{
    uint32_t length = 0;
    while (*str)
    {
        length++;
        str++;
    }
    return length;
}

char *strcpy(char *dest, const char *src)
{
    uint32_t i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

int strcmp(char *str1, const char *str2)
{
    while (*str1 != '\0' && *str2 != '\0')
    {
        if (*str1 != *str2)
        {
            return (str1 - str2);
        }
        str1++;
        str2++;
    }
    return 0;
}

int findch(uint32_t start_index, char target, char *str)
{
    for (uint32_t i = start_index; str[i] != '\0'; i++)
    {
        if (str[i] == target)
        {
            return i;
        }
    }
    return -1;
}

int str_to_upper(int c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 32;
    }
    return c;
}

int str_to_lower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + 32;
    }
    return c;
}

uint32_t str_to_uint(char *str)
{
    uint32_t res = 0;
    while (*str >= '0' && *str <= '9')
    {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

void reverse_str(char *str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }

    int start = 0;
    int end = len - 1;

    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

bool check_eq_of_strings(char *s1, char *s2)
{
    return strcmp(s1, s2) == 0 && strlen(s1) == strlen(s2);
}

// Функция взята из https://github.com/0Nera/BMOSP
// Преобразование целого числа "i" в системе счисления "base" в строку "buf"
void int_to_str(int32_t i, uint8_t base, char *buf)
{
    bool negative = false;

    // Проверяем, является ли число отрицательным
    if (i < 0)
    {
        negative = true;
        i *= -1; // Если да, то умножаем на -1, чтобы сделать его положительным
    }

    int32_t index = 0;
    // Деление с остатком для преобразования числа в нужную систему счисления
    do
    {
        int32_t remainder = i % base;
        // Преобразовываем остаток в символ и добавляем его в строку
        buf[index++] =
            (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0'; // Если остаток > 9, добавляем заглавную букву А
        i /= base;
    } while (i > 0);

    // Если число было отрицательным, добавляем знак минуса в конец строки
    if (negative)
    {
        buf[index++] = '-';
    }

    // Добавляем нулевой символ в конец строки, чтобы завершить ее
    buf[index] = '\0';

    // Переворачиваем строку, чтобы цифры были в правильном порядке
    reverse_str(buf);
}

// Функция взята из https://github.com/0Nera/BMOSP
// Преобразование неотрицательного целого числа "i" в системе счисления "base" в строку "buf"
void uint_to_str(uint32_t i, uint8_t base, char *buf)
{
    uint32_t index = 0;
    // Деление с остатком для преобразования числа в нужную систему счисления
    do
    {
        uint32_t remainder = i % base;
        // Преобразовываем остаток в символ и добавляем его в строку
        buf[index++] =
            (remainder > 9)
                ? (remainder - 10) + 'A'
                : remainder +
                      '0'; // Если остаток > 9, добавляем заглавную букву А
        i /= base;
    } while (i > 0);

    // Добавляем нулевой символ в конец строки, чтобы завершить ее
    buf[index] = '\0';

    // Переворачиваем строку, чтобы цифры были в правильном порядке
    reverse_str(buf);
}

uint16_t tflip_uint16(uint16_t num)
{
    uint16_t n_buff[2];
    n_buff[0] = *((uint8_t *)(&num));
    n_buff[1] = *((uint8_t *)(&num) + 1);
    return (n_buff[0] << 8) | (n_buff[1]);
}