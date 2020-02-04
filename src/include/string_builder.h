#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SB_STR_FRAG_LENGTH 32

typedef struct {
  char *str;
  int length;
  int __allocated;

} string_builder_t;

string_builder_t *sb_init();

bool sb_is_empty(string_builder_t *sb);

bool sb_append(string_builder_t *sb, const char *str);

bool sb_append_char(string_builder_t *sb, char ch);

char *sb_build_str(string_builder_t *sb);

void sb_reset(string_builder_t *sb);

void sb_free(string_builder_t *sb);

#endif /* STRING_BUILDER_H */

