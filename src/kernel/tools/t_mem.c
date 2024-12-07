#include <tools.h> // Include header for utility functions

// Copy 'n' bytes from source to destination
void tmemcpy(void *dest, void *src, uint32_t n) {
    char* d = (char*)dest; // Cast destination to char pointer
    const char* s = (const char*)src; // Cast source to const char pointer

    for (uint32_t i = 0; i < n; i++) {
        d[i] = s[i]; // Copy byte by byte
    }
}

// Copy 'n' words (16 bits) from source to destination
void tmemcpyw(void *dest, void *src, uint32_t n) {
    uint16_t* d = (uint16_t*)dest; // Cast destination to uint16_t pointer
    const uint16_t* s = (const uint16_t*)src; // Cast source to const uint16_t pointer

    for (uint16_t i = 0; i < n; i++) {
        d[i] = s[i]; // Copy word by word
    }
}

// Copy 'n' double words (32 bits) from source to destination
void tmemcpyd(void *dest, void *src, uint32_t n) {
    uint32_t* d = (uint32_t*)dest; // Cast destination to uint32_t pointer
    const uint32_t* s = (const uint32_t*)src; // Cast source to const uint32_t pointer

    for (uint32_t i = 0; i < n; i++) {
        d[i] = s[i]; // Copy double word by double word
    }
}

// Reverse the elements of an array of double words in place
void tmemcpyd_inv(void *arr, uint32_t n) {
    uint32_t *darr = (uint32_t *)arr; // Cast array to uint32_t pointer

    int start = 0; // Start index
    int end = n - 1; // End index

    while (start < end) { // Swap elements until the middle is reached
        int temp = darr[start];
        darr[start] = darr[end];
        darr[end] = temp;

        start++;
        end--;
    }
}

// Compare 'n' bytes of two memory areas and return 0 if equal, -1 if s1 < s2, 1 if s1 > s2
int tmemcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = (unsigned char *)s1; // Cast s1 to unsigned char pointer
    const unsigned char *p2 = (unsigned char *)s2; // Cast s2 to unsigned char pointer

    for (size_t i = 0; i < n; i++) { // Compare byte by byte
        if (p1[i] < p2[i]) {
            return -1; // Return -1 if s1 is less than s2
        } else if (p1[i] > p2[i]) {
            return 1; // Return 1 if s1 is greater than s2
        }
    }

    return 0; // Return 0 if both are equal
}

// Set 'size' bytes of memory pointed by ptr to the value 'n'
void *tmemset(void *ptr, uint8_t n, uint32_t size) {
    uint8_t *p = (uint8_t *)ptr; // Cast ptr to uint8_t pointer
    for (uint32_t i = 0; i < size; i++) {
        p[i] = n; // Set each byte to value n
    }
    return ptr; // Return the original pointer
}

// Set 'size' words of memory pointed by ptr to the value 'n'
void *tmemsetw(void *ptr, uint16_t n, uint32_t size) {
    uint16_t *p = (uint16_t *)ptr; // Cast ptr to uint16_t pointer
    for (uint32_t i = 0; i < size; i++) {
        p[i] = n; // Set each word to value n
    }
    return ptr; // Return the original pointer
}

// Set 'size' double words of memory pointed by ptr to the value 'n'
void *tmemsetd(void *ptr, uint32_t n, uint32_t size) {
    uint32_t *p = (uint32_t *)ptr; // Cast ptr to uint32_t pointer
    for (uint32_t i = 0; i < size; i++) {
        p[i] = n; // Set each double word to value n
    }
    return ptr; // Return the original pointer
}

void * memchr(const void * src, int c, size_t n) {
	const unsigned char * s = src;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & (ALIGN - 1)) && n && *s != c; s++, n--);
	if (n && *s != c) {
		const size_t * w;
		size_t k = ONES * c;
		for (w = (const void *)s; n >= sizeof(size_t) && !HASZERO(*w^k); w++, n -= sizeof(size_t));
		for (s = (const void *)w; n && *s != c; s++, n--);
	}
	return n ? (void *)s : 0;
}

// Structure used for fast copy operation with a dummy array of bytes
typedef struct {
    unsigned char dummy[32]; 
} __DT;

// Fast copy function that uses unaligned access for speed optimization
void tfastcpy(unsigned char *dst, unsigned char *src, size_t s) {
    unsigned char *sa = src + s;
    __DT *d1 = (__DT *)dst - 1;
    __DT *s1 = (__DT *)src - 1;
    
    size_t si = s / sizeof(__DT); // Calculate how many __DT structures fit into the size

    si = (si + 7) / 8;
    
    switch (si % 8) { // Use unrolled loop based on si modulo 8 for performance optimization
        case 0:
            do {
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

    while (src < sa) { 
        *++dst = *++src; // Copy remaining bytes one by one after unrolled loop 
    }
}

// Assembly function to execute CPUID instruction and retrieve processor information based on leaf value.
void _cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
    asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf), "c"(0));
}
