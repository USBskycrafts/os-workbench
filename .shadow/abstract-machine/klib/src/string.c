#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  size_t cnt = 0;
  const char *ptr = s;
  while (*ptr)
  {
    ptr++;
    cnt++;
  }
  return cnt;
}

char *strcpy(char *dst, const char *src)
{
  return strncpy(dst, src, INT32_MAX);
}

char *strncpy(char *dst, const char *src, size_t n)
{
  const char *ptr_s = src;
  char *ptr_d = dst;
  for (; *ptr_s && n--; ptr_d++, ptr_s++)
  {
    *ptr_d = *ptr_s;
  }
  return dst;
}

char *strcat(char *dst, const char *src)
{
  size_t n = strlen(dst);
  char *ptr_d = &dst[n];
  const char *ptr_s = src;
  while (*ptr_s)
  {
    *ptr_d = *ptr_s;
    ptr_d++;
    ptr_s++;
  }
  *ptr_d = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2)
{
  panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n)
{
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n)
{
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n)
{
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  panic("Not implemented");
}

#endif
