#include <tools.h>

#include <mem.h>

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

int strcmp(const char *a, const char *b)
{
    uint32_t i = 0;
	while (1) {
		if (a[i] < b[i]) {
			return -1;
		} else if (a[i] > b[i]) {
			return 1;
		} else {
			if (a[i] == '\0') {
				return 0;
			}
			++i;
		}
	}
}

int strncmp(const char *s1, const char *s2, size_t n) {
    size_t i = 0;

    while (i < n && s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (uint8_t)s1[i] - (uint8_t)s2[i];
        }
        i++;
    }

    if (i < n) {
        if (s1[i] != s2[i]) {
            return (uint8_t)s1[i] - (uint8_t)s2[i];
        }
    }

    return 0;
}

const char* strrchr(const char* str, int target) {
    const char* last = str;
    for (uint32_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == target)
        {
            last = &((char*)str)[i];
        }
    }
    return last;
}

char * strchrnul(const char * s, int c) {
	size_t * w;
	size_t k;

	c = (unsigned char)c;
	if (!c) {
		return (char *)s + strlen(s);
	}

	for (; (uintptr_t)s % ALIGN; s++) {
		if (!*s || *(unsigned char *)s == c) {
			return (char *)s;
		}
	}

	k = ONES * c;
	for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
	for (s = (void *)w; *s && *(unsigned char *)s != c; s++);
	return (char *)s;
}

char * strchr(const char * s, int c) {
	char *r = strchrnul(s, c);
	return *(unsigned char *)r == (unsigned char)c ? r : 0;
}

size_t strcspn(const char * s, const char * c) {
	const char *a = s;
	if (c[0] && c[1]) {
		size_t byteset[32/sizeof(size_t)] = { 0 };
		for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
		for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++);
		return s-a;
	}
	return strchrnul(s, *c)-a;
}

char * strpbrk(const char * s, const char * b) {
	s += strcspn(s, b);
	return *s ? (char *)s : 0;
}

size_t strspn(const char * s, const char * c) {
	const char * a = s;
	size_t byteset[32/sizeof(size_t)] = { 0 };

	if (!c[0]) {
		return 0;
	}
	if (!c[1]) {
		for (; *s == *c; s++);
		return s-a;
	}

	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
	for (; *s && BITOP(byteset, *(unsigned char *)s, &); s++);

	return s-a;
}

size_t lfind(const char * str, const char accept) {
	return (size_t)strchr(str, accept);
}

size_t rfind(const char * str, const char accept) {
	return (size_t)strrchr(str, accept);
}

char * strtok_r(char * str, const char * delim, char ** saveptr) {
	char * token;
	if (str == NULL) {
		str = *saveptr;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		*saveptr = str;
		return NULL;
	}
	token = str;
	str = strpbrk(token, delim);
	if (str == NULL) {
		*saveptr = (char *)lfind(token, '\0');
	} else {
		*str = '\0';
		*saveptr = str + 1;
	}
	return token;
}

char * strtok(char * str, const char * delim) {
	static char * saveptr = NULL;
	if (str) {
			saveptr = NULL;
	}
	return strtok_r(str, delim, &saveptr);
}

char * strcat(char *dest, const char *src) {
	char * end = dest;
	while (*end != '\0') {
		++end;
	}
	while (*src) {
		*end = *src;
		end++;
		src++;
	}
	*end = '\0';
	return dest;
}

char * strncat(char *dest, const char *src, size_t n) {
	char * end = dest;
	while (*end != '\0') {
		++end;
	}
	size_t i = 0;
	while (*src && i < n) {
		*end = *src;
		end++;
		src++;
		i++;
	}
	*end = '\0';
	return dest;
}

static char *strstr_2b(const unsigned char * h, const unsigned char * n) {
	uint16_t nw = n[0] << 8 | n[1];
	uint16_t hw = h[0] << 8 | h[1];
	for (h++; *h && hw != nw; hw = hw << 8 | *++h);
	return *h ? (char *)h-1 : 0;
}

static char *strstr_3b(const unsigned char * h, const unsigned char * n) {
	uint32_t nw = n[0] << 24 | n[1] << 16 | n[2] << 8;
	uint32_t hw = h[0] << 24 | h[1] << 16 | h[2] << 8;
	for (h += 2; *h && hw != nw; hw = (hw|*++h) << 8);
	return *h ? (char *)h-2 : 0;
}

static char *strstr_4b(const unsigned char * h, const unsigned char * n) {
	uint32_t nw = n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
	uint32_t hw = h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
	for (h += 3; *h && hw != nw; hw = hw << 8 | *++h);
	return *h ? (char *)h-3 : 0;
}

static char *strstr_twoway(const unsigned char * h, const unsigned char * n) {
	size_t mem;
	size_t mem0;
	size_t byteset[32 / sizeof(size_t)] = { 0 };
	size_t shift[256];
	size_t l;

	/* Computing length of needle and fill shift table */
	for (l = 0; n[l] && h[l]; l++) {
		BITOP(byteset, n[l], |=);
		shift[n[l]] = l+1;
	}

	if (n[l]) {
		return 0; /* hit the end of h */
	}

	/* Compute maximal suffix */
	size_t ip = -1;
	size_t jp = 0;
	size_t k = 1;
	size_t p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else {
				k++;
			}
		} else if (n[ip+k] > n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	size_t ms = ip;
	size_t p0 = p;

	/* And with the opposite comparison */
	ip = -1;
	jp = 0;
	k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else {
				k++;
			}
		} else if (n[ip+k] < n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip+1 > ms+1) {
		ms = ip;
	} else {
		p = p0;
	}

	/* Periodic needle? */
	if (tmemcmp(n, n+p, ms+1)) {
		mem0 = 0;
		p = MAX(ms, l-ms-1) + 1;
	} else {
		mem0 = l-p;
	}
	mem = 0;

	/* Initialize incremental end-of-haystack pointer */
	const unsigned char * z = h;

	/* Search loop */
	for (;;) {
		/* Update incremental end-of-haystack pointer */
		if ((size_t)(z-h) < l) {
			/* Fast estimate for MIN(l,63) */
			size_t grow = l | 63;
			const unsigned char *z2 = memchr(z, 0, grow);
			if (z2) {
				z = z2;
				if ((size_t)(z-h) < l) {
					return 0;
				}
			} else {
				z += grow;
			}
		}

		/* Check last byte first; advance by shift on mismatch */
		if (BITOP(byteset, h[l-1], &)) {
			k = l-shift[h[l-1]];
			if (k) {
				if (mem0 && mem && k < p) k = l-p;
				h += k;
				mem = 0;
				continue;
			}
		} else {
			h += l;
			mem = 0;
			continue;
		}

		/* Compare right half */
		for (k=MAX(ms+1,mem); n[k] && n[k] == h[k]; k++);
		if (n[k]) {
			h += k-ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k=ms+1; k>mem && n[k-1] == h[k-1]; k--);
		if (k <= mem) {
			return (char *)h;
		}
		h += p;
		mem = mem0;
	}
}

char *strstr(const char * h, const char * n) {
	/* Return immediately on empty needle */
	if (!n[0]) {
		return (char *)h;
	}

	/* Use faster algorithms for short needles */
	h = strchr(h, *n);
	if (!h || !n[1]) {
		return (char *)h;
	}

	if (!h[1]) return 0;
	if (!n[2]) return strstr_2b((void *)h, (void *)n);
	if (!h[2]) return 0;
	if (!n[3]) return strstr_3b((void *)h, (void *)n);
	if (!h[3]) return 0;
	if (!n[4]) return strstr_4b((void *)h, (void *)n);

	/* Two-way on large needles */
	return strstr_twoway((void *)h, (void *)n);
}

uint8_t startswith(const char * str, const char * accept) {
	return strstr(str, accept) == str;
}

char* strdup(const char* s) {
    char* news = kmalloc(strlen(s) + sizeof(size_t));
    strcpy(news, s);
    return news;
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