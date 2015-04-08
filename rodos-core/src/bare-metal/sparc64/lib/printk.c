/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Kernel printf for sparc64.
 *
 */

#include <stddef.h>
#include <stdarg.h>
#include <printk.h>
#include <string_pico.h>

/**
 * @brief Internal function to put a number x to base b.
 * This function is not visible externally.
 *
 * @param x Number
 * @param b Base
 * @param width Width of the padding before the number.
 * @param fill What the padding is to be filled with.
 *
 * @return
 */
int _putbase(uint64_t x, int b, int width, char fill) {
  char hex[] = "0123456789abcdef"; /* converter */
  char buffer[64];
  char *bptr = buffer;
  uint64_t i, putdash=0;
  int n = 0;

  memset(buffer, 64, 0);

  /* if negative, put a '-' sign and make it positive */
  if (b == 10 && (long) x < 0) {
    putdash = 1;
    x = -x;
  }

  /* do {} while; since we also want to print a '0'
   * we do hex[x%b] so it works for all bases below 16
   */
  do {
    i = x % b;
    *bptr++ = hex[i];
    x -= i;
    x /= b;
  } while (x);

  /* since we put in reverse order, add the dash at last */
  if (putdash)
    *bptr++ = '-';

  /* first print the padding */
  for (width -= (bptr-buffer); width > 0; --width) {
    putchar(fill);
    n++;
  }

  /* then take the actual stuff out of the buffer */
  for (--bptr; bptr >= buffer; --bptr) {
    putchar(*bptr);
    n++;
  }

  return n;
}

/**
 * @brief Subset of vprintf sufficient enough for kernel printing.
 *
 * @param fmt String that contains the text to be written to stdout.
 * @param arg Variable argument list.
 *
 * @return Number of characters printed.
 */
int _printk(const char *fmt, va_list arg) {
  uint64_t x;
  char c, *sptr;
  int n = 0;
  int width;
  char fill;

  while ((c = *fmt++)) {
    if (c != '%') {
      putchar(c);
      n++;
      continue;
    }

    c = *fmt++; /* next */

    fill = ' ';
    if (c == '0') {
      fill = c;
      c = *fmt++; /* next */
    }

    width = 0;
    while (c >= '0' && c <= '9') {
      width  = (10*width);
      width += (c-'0');
      c = *fmt++; /* next */
    }

    switch (c) {
      case 'b':
        x = va_arg(arg, uint64_t);
        n += _putbase(x, 2, width, fill);
        break;

      case 'c':
        c = va_arg(arg, int);
        putchar(c);
        n++;
        break;

      case 'd':
        x = va_arg(arg, uint64_t);
        n += _putbase(x, 10, width, fill);
        break;

      case 'o':
        x = va_arg(arg, uint64_t);
        n += _putbase(x, 8, width, fill);
        break;

      case 's':
        sptr = va_arg(arg, char *);
        while (*sptr) {
          putchar(*sptr++);
          n++;
        }
        break;

      case 'x':
        x = va_arg(arg, uint64_t);
        n += _putbase(x, 16, width, fill);
        break;

      default:
        putchar(c);
        n++;
        break;
    }
  }
  return n;
}

/**
 * @brief Print formatted data to stdout.
 *
 * @param fmt String that contains the text to be written to stdout.
 * @param ... The function may expect a sequence of additional arguments.
 *
 * @return Number of characters printed.
 */
int printk(const char *fmt, ...) {
  va_list arg;
  int n;

  va_start(arg, fmt);
  n = _printk(fmt, arg);
  va_end(arg);
  return n;
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
