#include "./include/string_builder.h"

string_builder_t *sb_init() {
  string_builder_t *sb =
      (string_builder_t *)calloc(1, sizeof(string_builder_t));

  sb->length = 0;
  sb->str = (char *)malloc(sizeof(char) * SB_STR_FRAG_LENGTH);
  sb->__allocated = sizeof(char) * SB_STR_FRAG_LENGTH;
  *sb->str = '\0';

  return sb;
}

bool sb_is_empty(string_builder_t *sb) {
  return sb->length == 0 || sb->str == NULL;
}

bool sb_append(string_builder_t *sb, const char *str) {
  if (str == NULL) {
    return false;
  }

  int len = strlen(str);

  // Add space for a string, if needed
  while (sb->__allocated < sb->length + len + 1) {
    sb->__allocated *= 2;
  }

  sb->str = (char *) realloc(sb->str, sb->__allocated);
  memmove(sb->str + sb->length, str, len);
  sb->length += len;
  sb->str[sb->length] = '\0';

  return true;
}

char *sb_build_str(string_builder_t *sb) { return sb->str; }

void sb_reset(string_builder_t *sb) {
  sb->str = NULL;
  sb->length = 0;
}

void sb_free(string_builder_t *sb) {
  free(sb->str);
  free(sb);
}

