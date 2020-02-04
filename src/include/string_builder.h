#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SB_MAX_FRAG_LENGTH 4096
#define SB_FAILURE -1

static const size_t STRING_MIN_SIZE = 32;

struct string_fragment_t {
  struct string_fragment_t *next;
  int length;
  char *str;
};

typedef struct string_fragment_t string_fragment_t;

struct string_builder_t {
  string_fragment_t *root;
  string_fragment_t *trunk;
  int length;
};

typedef struct string_builder_t string_builder_t;

/**
 *
 */
string_builder_t *sb_init();

bool sb_is_empty(string_builder_t *sb);

bool sb_append(string_builder_t *sb, const char *str);

bool sb_append_char(string_builder_t *sb, char ch);

int sb_appendf(string_builder_t *sb, const char *fmt, ...);

char *sb_build_str(string_builder_t *sb);

void sb_reset(string_builder_t *sb);

void sb_free(string_builder_t *sb);

#endif /* STRING_BUILDER_H */
