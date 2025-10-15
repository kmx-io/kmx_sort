/* sort
 * Copyright 2025 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use, modify and redistribute this
 * software or modified versions of this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct str {
  char  *str_ptr;
  size_t str_size;
} s_str;

static void error (const char *message);
static int str_compare (const void *a, const void *b);
static void usage (void);

static const char *g_argv0 = "sort";
static int         g_unique = 0;

static void error (const char *message)
{
  fputs(message, stderr);
  exit(1);
}

int main (int argc, char **argv)
{
  size_t i;
  char   *line;
  ssize_t line_len;
  size_t  line_size;
  s_str *str;
  size_t str_count;
  size_t str_max;
  s_str *str2;
  size_t str2_max;
  if (! argc || ! argv || ! argv[0])
    usage();
  g_argv0 = argv[0];
  if (argc > 1) {
    if (argc > 2)
      usage();
    if (argv[1][0] == '-' &&
        argv[1][1] == 'u' &&
        ! argv[1][2])
      g_unique = 1;
    else
      usage();
  }
  str_max = 512;
  str = calloc(str_max, sizeof(s_str));
  i = 0;
  while (1) {
    line = NULL;
    line_size = 0;
    if ((line_len = getline(&line, &line_size, stdin)) <= 0)
      break;
    if (line[line_len - 1] == '\n') {
      line[--line_len] = 0;
      if (line_len > 0 && line[line_len - 1] == '\r')
        line[--line_len] = 0;
    }
    if (i >= str_max) {
      str2_max = str_max + 512;
      str2 = calloc(str2_max, sizeof(s_str));
      memcpy(str2, str, str_max * sizeof(s_str));
      free(str);
      str = str2;
      str_max = str2_max;
    }
    str[i].str_ptr = line;
    str[i].str_size = line_len;
    i++;
  }
  if (ferror(stdin) || ! feof(stdin))
    error("sort: getline");
  str_count = i;
  if (str_count) {
    qsort(str, str_count, sizeof(str[0]), str_compare);
    i = 0;
    while (i < str_count) {
      if (! g_unique || ! i || str_compare(str + i - 1, str + i)) {
	if ((str[i].str_size &&
	     fwrite(str[i].str_ptr, str[i].str_size, 1, stdout) <= 0) ||
	    fwrite("\n", 1, 1, stdout) <= 0)
	  error("sort: fwrite stdout");
      }
      i++;
    }
    fflush(stdout);
  }
  return 0;
}

static int str_compare (const void *a, const void *b)
{
  size_t len;
  int r;
  const s_str *str_a;
  const s_str *str_b;
  str_a = a;
  str_b = b;
  len = (str_a->str_size < str_b->str_size) ?
    str_a->str_size :
    str_b->str_size;
  if ((r = memcmp(str_a->str_ptr, str_b->str_ptr, len)))
    return r;
  if (str_a->str_size < str_b->str_size)
    return -1;
  if (str_a->str_size > str_b->str_size)
    return 1;
  return 0;
}

static void usage (void)
{
  fprintf(stderr, "Usage: %s [-u]\n", g_argv0);
  exit(1);
}
