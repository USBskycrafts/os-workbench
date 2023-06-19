#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  char buf[4096];
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(buf, 4095, fmt, ap);
  va_end(ap);
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  return vsnprintf(out, INT32_MAX, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  enum
  {
    idle,
    specifier
  } flag;
  flag = idle;
  int cnt = 0;
  const char *p = fmt;
  for (; *p && n; p++)
  {
    if (*p == '%' && flag == idle)
    {
      flag = specifier;
      continue;
    }
    if (flag == specifier)
    {
      flag = idle;
      switch (*p)
      {
      case 'd':
      case 'i':
      {
        char num[34];
        itoa(num, va_arg(ap, int));
        char *ptr = num;
        while (*ptr)
        {
          out[cnt] = *ptr;
          cnt++;
          ptr++;
        }
        break;
      }
      case 'u':
      {
        char num[33];
        utoa(num, va_arg(ap, unsigned));
        char *ptr = num;
        while (*ptr)
        {
          out[cnt] = *ptr;
          cnt++;
          ptr++;
        }
        break;
      }
      case 's':
      {
        char *s = va_arg(ap, char *);
        while (*s)
        {
          out[cnt] = *s;
          cnt++;
          n--;
        }
        break;
      }
      case 'c':
      {
        out[cnt++] = va_arg(ap, int);
        break;
      }
      case '%':
      {
        out[cnt++] = '%';
        break;
      }
      default:
        goto error;
        break;
      }
    }
  }
  return cnt;
error:
  return -1;
}

#endif
