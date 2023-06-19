#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void)
{
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed)
{
  next = seed;
}

int abs(int x)
{
  return (x < 0 ? -x : x);
}

int atoi(const char *nptr)
{
  int x = 0;
  while (*nptr == ' ')
  {
    nptr++;
  }
  while (*nptr >= '0' && *nptr <= '9')
  {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

void itoa(char *ptr, long long val)
{
  if (val >= 0)
  {
    utoa(ptr, val);
  }
  else
  {
    ptr[0] = '-';
    unsigned long long tmp = ~(unsigned long long)val + 1;
    utoa(ptr + 1, tmp);
  }
}

void utoa(char *ptr, unsigned long long val)
{
  size_t cnt = 0;
  while (val)
  {
    ptr[cnt++] = val % 10;
    val /= 10;
  }
  for (int i = 0; i < cnt / 2; i++)
  {
    char tmp = ptr[i];
    assert((int)cnt - 1 - i >= 0);
    ptr[cnt - 1 - i] = tmp;
    ptr[i] = ptr[cnt - 1 - i];
  }
  ptr[cnt] = '\0';
}

void *malloc(size_t size)
{
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  panic("Not implemented");
#endif
  return NULL;
}

void free(void *ptr)
{
}

#endif
