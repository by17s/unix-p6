#include <tools.h>

void tmemcpy(void *dest, void *src, uint32_t n)
{
    char* d = (char*)dest;
    const char* s = (const char*)src;

    for (uint32_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}

void tmemcpyw(void *dest, void *src, uint32_t n)
{
    uint16_t* d = (uint16_t*)dest;
    const uint16_t* s = (const uint16_t*)src;

    for (uint16_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}

void tmemcpyd(void *dest, void *src, uint32_t n)
{
    uint32_t* d = (uint32_t*)dest;
    const uint32_t* s = (const uint32_t*)src;

    for (uint32_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}

void tmemcpyd_inv(void *arr, uint32_t n)
{
    uint32_t *darr = (uint32_t *)arr;

    int start = 0;
    int end = n;

    while (start < end)
    {
        int temp = darr[start];
        darr[start] = darr[end];
        darr[end] = temp;

        start++;
        end--;
    }
    
}

int tmemcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = (unsigned char *)s1;
    const unsigned char *p2 = (unsigned char *)s2;

    // Сравнение данных
    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] < p2[i])
        {
            return -1;
        }
        else if (p1[i] > p2[i])
        {
            return 1;
        }
    }

    return 0;
}

void *tmemset(void *ptr, uint8_t n, uint32_t size)
{
    uint8_t *p = (uint8_t *)ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        p[i] = n;
    }
    return ptr;
}

void *tmemsetw(void *ptr, uint16_t n, uint32_t size)
{
    uint16_t *p = (uint16_t *)ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        p[i] = n;
    }
    return ptr;
}

void *tmemsetd(void *ptr, uint32_t n, uint32_t size)
{
    uint32_t *p = (uint32_t *)ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        p[i] = n;
    }
    return ptr;
}

typedef struct
{
    unsigned char dummy[32];
} __DT;

void tfastcpy(unsigned char *dst, unsigned char *src, size_t s)
{
    unsigned char *sa = src + s;
    __DT *d1 = (__DT *)dst - 1;
    __DT *s1 = (__DT *)src - 1;
    size_t si = s / sizeof(__DT);

    si = (si + 7) / 8;
    switch (si % 8)
    {
    case 0:
        do
        {
            *++d1 = *++s1;
        case 7:
            *++d1 = *++s1;
        case 6:
            *++d1 = *++s1;
        case 5:
            *++d1 = *++s1;
        case 4:
            *++d1 = *++s1;
        case 3:
            *++d1 = *++s1;
        case 2:
            *++d1 = *++s1;
        case 1:
            *++d1 = *++s1;
        } while (--si > 0);
    }

    dst = (unsigned char *)d1;
    src = (unsigned char *)s1;
    while (src < sa)
    {
        *++dst = *++src;
    }
}

void _cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf), "c"(0));
}