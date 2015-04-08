
#include "string_pico.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif
extern "C" {

size_t strlen(const char *s) {
    int len = 0;
    if(!s) return 0;
    while(*s) {
      len++;
      s++;
    }
    return len;
}

size_t strnlen(const char *s, size_t n) {
    unsigned int len = 0;
    if(!s) return 0;
    while(*s) {
      len++;
      s++;
      if(len >= n) return len;
    }
    return len;
}


char* strcpy(char* destP, const char *s) {
    char* dest = destP;
    if(!s || !dest) return destP;
    while(*s) {
      *dest = *s;
      dest++;
      s++;
    }
    *dest = 0;
    return destP;
}


void* memset(void* destP, int val, size_t len) {
    if(!destP) return destP ;
    char* dest = (char*)destP;
    while(len) {
      *dest = val;
      dest++;  // Added
      len--;
    }
    return destP;
}

void* memcpy(void* destP, const void* sP, size_t len) {
    char* dest = (char*)destP;
    char* s    = (char*)sP;
    if(!dest || !s) return 0;
    while(len) {
	// dest[len] = s[len];
      *dest = *s;
      len--;
      s++;
      dest++;
    }
    return destP;
}
	
int memcmp(const void* a, const void* b, size_t len) {
    unsigned char* cA = (unsigned char*)a;
    unsigned char* cB = (unsigned char*)b;

    while(len) {
      if (*cA < *cB) return -1;
      if (*cA > *cB) return 1;
      cA++;
      cB++;
      len--;
    }
    return 0;
}



//FILE *stdin=  0;
//FILE *stdout= 0;
//FILE *stderr= 0;

//int __errno;

//void* memcpyXXXX(void* dest, const void* src, int n)
//{ return memmove (dest, src, n); /* principle simplicity: use memmove */ }

char* strncpy(char* destP, const char* src, size_t n) {
  char* dest = destP;
  if(!dest || !src) return 0;
  while (n > 0) {
    *dest = *src;
    if (*src == 0)
      break;
    src++;
    dest++;
    n--; 
  }
  *dest = 0;
  return destP;
}

char* strcpyXXXX(char* dest, const char* src) {
  return (char *)memcpy (dest, src, strlen(src)+1);
}

int strcmp (char const *a, char const *b) {
  if (a == 0)
    return b == 0 ? 0 : -1;

  if (b == 0)
    return +1;

  unsigned char ac, bc;
  do {
    ac= (unsigned char) *a++;
    bc= (unsigned char) *b++;
    if (ac < bc)  return -1;
    if (ac > bc)  return +1;
  }
  while (ac);

  return 0;
}

int strncmp (char const *a, char const *b, size_t n) {
  if (a == 0)
    return b == 0 ? 0 : -1;

  if (b == 0)
    return +1;

  unsigned char ac, bc;
  do {
    ac= (unsigned char) *a++;
    bc= (unsigned char) *b++;
    if (n == 0) return 0;
    n--; // Explizites Inkrement, da dies sonst auch gemacht wird.
    if (ac < bc)  return -1;
    if (ac > bc)  return +1;
  } while (ac);
  // while (ac && bc); 
// Nach den beiden ifs bleibt nur ac == bc, ist ein String kürzer kommen wir
// hier nicht mehr an.

  return 0;
}

} // extern "C"

#ifndef NO_RODOS_NAMESPACE
}
#endif

/**
 * C++ stuff: operator delete.
 * We cannot delete. */
void operator delete (void *) { }


