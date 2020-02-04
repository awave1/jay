#include "./include/string_builder.h"

string_builder_t *sb_init() {
  string_builder_t *sb =
      (string_builder_t *)calloc(sizeof(string_builder_t), 1);
  return sb;
}

bool sb_is_empty(string_builder_t *sb) { return (sb->root == NULL); }

bool sb_append(string_builder_t *sb, const char *str) {
  int length = strlen(str);
  string_fragment_t *fragment = (string_fragment_t *)malloc(
      sizeof(string_fragment_t) + sizeof(char) * length);

  if (fragment == NULL) {
    return false;
  }

  fragment->next = NULL;
  fragment->length = length;

  // copy str to fragment->str
  memcpy((void *)&fragment->str, (const void *)str,
         sizeof(char) * (length + 1));

  sb->length += length;

  if (sb->root == NULL) {
    sb->root = fragment;
  } else {
    sb->trunk->next = fragment;
  }

  sb->trunk = fragment;

  return true;
}

bool sb_append_char(string_builder_t *sb, char ch) {
  char str[1] = {ch};
  return sb_append(sb, str);
}

char *sb_build_str(string_builder_t *sb) {
  // allocate memory for the string
  // which will be the size of all strings in string vuilder (sb->length)
  char *strbuf = (char *)malloc((sb->length + 1) * sizeof(char));

  if (strbuf == NULL) {
    return NULL;
  }

  // use temp container that points to strbuf
  char *container = strbuf;
  for (string_fragment_t *fragment = sb->root; fragment != NULL;
       fragment = fragment->next) {
    // copt fragment to string buffer
    memcpy(container, &fragment->str, sizeof(char) * fragment->length);
    container += fragment->length;
  }

  *container = '\0';
  return strbuf;
}

void sb_reset(string_builder_t *sb) {
  string_fragment_t *fragment = sb->root;
  string_fragment_t *next = NULL;

  while (fragment != NULL) {
    next = fragment->next;
    free(fragment);
    fragment = next;
  }

  sb->root = NULL;
  sb->trunk = NULL;
  sb->length = 0;
}

void sb_free(string_builder_t *sb) { free(sb); }
