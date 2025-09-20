/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
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

typedef struct rope s_rope;

struct rope {
  char *str;
  size_t len;
  s_rope *next;
};

static void error (const char *message);
static int       rope_compare (const s_rope *a, const s_rope *b);
static s_rope *  rope_delete (s_rope *rope);
static void      rope_delete_all (s_rope *rope);
static s_rope ** rope_insert_sorted (s_rope **rope, char *str,
                                     size_t len);
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
  s_rope *rope = NULL;
  char *str;
  size_t str_len;
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
  }
  while (1) {
    str = NULL;
    str_len = 0;
    if (getline(&str, &str_len, stdin) <= 0)
      break;
    if (str[str_len - 1] == '\n') {
      str[--str_len] = 0;
      if (str[str_len - 1] == '\r')
        str[--str_len] = 0;
    }
    if (! rope_insert_sorted(&rope, str, str_len))
      error("sort: rope_insert_sorted");
  }
  if (ferror(stdin) || ! feof(stdin)) {
    rope_delete_all(rope);
    error("sort: getline");
  }
  while (rope) {
    if (fputs(rope->str, stdout) < 0) {
      rope_delete_all(rope);
      error("sort: fputs stdout");
    }
    rope = rope_delete(rope);
  }
  fflush(stdout);
  return 0;
}

static int rope_compare (const s_rope *a, const s_rope *b)
{
  size_t len;
  int r;
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  len = (a->len < b->len) ? a->len : b->len;
  if ((r = memcmp(a->str, b->str, len)))
    return r;
  if (a->len < b->len)
    return -1;
  if (a->len > b->len)
    return 1;
  return 0;
}

static s_rope * rope_delete (s_rope *rope)
{
  s_rope *next;
  next = rope->next;
  free(rope->str);
  free(rope);
  return next;
}

static void rope_delete_all (s_rope *rope)
{
  while (rope)
    rope = rope_delete(rope);
}

static s_rope ** rope_insert_sorted (s_rope **rope, char *str,
                                     size_t len)
{
  int c = 1;
  s_rope **cursor;
  s_rope *tmp;
  if (! (tmp = calloc(1, sizeof(s_rope))))
    error("sort: rope_new: calloc s_rope");
  tmp->str = str;
  tmp->len = len;
  cursor = rope;
  while (*cursor &&
         (c = rope_compare(*cursor, tmp)) < 0)
    cursor = &(*cursor)->next;
  if (g_unique) {
    if (c) {
      tmp->next = *cursor;
      *cursor = tmp;
    }
    else
      rope_delete(tmp);
  }
  else {
    tmp->next = *cursor;
    *cursor = tmp;
  }
  return rope;
}

static void usage (void)
{
  fprintf(stderr, "Usage: %s\n", g_argv0);
  exit(1);
}
