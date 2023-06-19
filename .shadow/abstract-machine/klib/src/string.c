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
  return strncmp(s1, s2, INT32_MAX);
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  const char *ptr_1 = s1;
  const char *ptr_2 = s2;
  for (; *ptr_1 && *ptr_2 && n--; ptr_1++, ptr_2++)
  {
    if (*ptr_1 != *ptr_2)
    {
      return false;
    }
  }
  return true;
}

void *memset(void *s, int c, size_t n)
{
  char *ptr = s;
  for (size_t i = 0; i < n; i++)
  {
    ptr[i] = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n)
{
  // dst and src may lap
  char tmp[n];
  const char *ptr_s = src;
  for (int i = 0; i < n; i++)
  {
    tmp[i] = *ptr_s;
    ptr_s++;
  }
  char *ptr_d = dst;
  for (int i = 0; i < n; i++)
  {
    *ptr_d = tmp[i];
    ptr_d++;
  }
  return dst;
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
