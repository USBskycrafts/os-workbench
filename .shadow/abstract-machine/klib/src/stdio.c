/**
 * @file stdio.c
 * @brief printf tested, 
 * @todo testing other function
 * @author Junran Yang
 * @copyright 2023 Copyright Junran Yang
 */

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buf[4100];
  static int spinlock = 0;
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(buf, 4096, fmt, ap);
  va_end(ap);
  while(atomic_xchg(&spinlock, 1));
  putstr(buf);
  spinlock = 0;
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, INT32_MAX, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  const char* ptr = fmt;
  bool drain = false;
  size_t cnt = 0;
  for(; *ptr && cnt < n; ptr++) {
    if(*ptr == '%') {
      drain = true;
      continue;
    }
    if(drain) {
      switch(*ptr) {
        case 'd': {
          int d = va_arg(ap, int);
          char buf[36];
          itoa(buf, d, 10);
          strcpy(&out[cnt], buf);
          cnt += strlen(buf);
          break;
        }
        case 'x': {
          int d = va_arg(ap, int);
          char buf[36];
          itoa(buf, d, 16);
          strcpy(&out[cnt], "0x");
          strcat(&out[cnt], buf);
          cnt += strlen(buf) + 2;
          break;
        }
        case 'p': {
          intptr_t p = va_arg(ap, intptr_t);
          char buf[36];
          itoa(buf, p, 16);
          strcpy(&out[cnt], "0x");
          strcat(&out[cnt], buf);
          cnt += strlen(buf) + 2;
          break;
        }
        case 's': {
          const char* s = va_arg(ap, char*);
          strcpy(&out[cnt], s);
          cnt += strlen(s);
          break;
        }
        case 'c': {
          char ch = va_arg(ap, uint32_t);
          out[cnt++] = ch;
          break;
        }
        default:
          panic("modifier can only be d, x, s, c");
      }
      drain = false;
    } else {
      out[cnt++] = *ptr;
    }
  }
  out[cnt] = '\0';
  return cnt;
}

#endif
