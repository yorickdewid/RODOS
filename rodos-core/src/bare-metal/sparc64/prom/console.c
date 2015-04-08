/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Use the OpenBoot PROM write and provide putchar.
 *
 */

#include <syscall.h>
#include <context.h>
#include <printk.h>
#include <oplib.h>

void clear_screen() {
  putchar(27);
  putchar('[');
  putchar('H');
  putchar(27);
  putchar('[');
  putchar('J');
}

int puts(const char *s) {
  int len = 0;

  while (s[len] != 0)
    len++;

  if(is_supervisor())
    sys_write(node_stdout, s, len);
  else
    write(node_stdout, s, len);

  return 1;
}

int putchar(int c) {
  char ch = c;

  if (c == '\n')
    putchar('\r');

  if(is_supervisor())
    sys_write(node_stdout, &ch, 1);
  else
    write(node_stdout, &ch, 1);

  return c;
}

/* temporary */
void sys_write(int fd, const char *buf, size_t nbytes) {
  unsigned int i;

  for (i = 0; i < nbytes; i++) {
    if (buf[i] == '\n')
      prom_write(fd, "\r", 1);
    prom_write(fd, &buf[i], 1);
  }
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
