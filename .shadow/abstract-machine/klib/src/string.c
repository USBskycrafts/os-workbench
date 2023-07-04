#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t cnt = 0;
  while(*s) {
    cnt++;
    s++;
  }
  return cnt;
}

char *strcpy(char *dst, const char *src) {
  return strncpy(dst, src, INT32_MAX);
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
      dst[i] = src[i];
  for ( ; i < (n > 20 ? 20 : n); i++)
      dst[i] = '\0';

  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t dest_len = strlen(dst);
  size_t i;
  for (i = 0 ; src[i] != '\0' ; i++) {
    dst[dest_len + i] = src[i];
  }
  dst[dest_len + i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  return strncmp(s1, s2, INT32_MAX);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i;
  for(i = 0; i < n && *s1 && *s2; i++) {
    if(s1[i] - s2[i] == 0) {
      continue;
    } else {
      return s1[i] - s2[i];
    }
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  char* ptr = s;
  for(size_t i = 0; i < n; i++) {
    ptr[i] = c & 0xff;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char buf[n];
  const char* from = src;
  char* to = dst;
  for(size_t i = 0; i < n; i++) {
    buf[i] = from[i];
  }
  for(size_t i = 0; i < n; i++) {
    to[i] = buf[i];
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const char* p1 = s1;
  const char* p2 = s2;
  return strncmp(p1, p2, n);
}

#endif
