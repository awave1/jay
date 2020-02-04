#include "./include/string_builder.h"
#include <stdio.h>

int main(int argc, char **argv) {
  string_builder_t *sb = sb_init();

  printf("is_empty %s\n", sb_is_empty(sb) ? "true" : "false");

  sb_append(sb, "YEEEUUR");

  char *str = sb_build_str(sb);

  printf("str: %s\n", str);
  printf("is_empty %s\n", sb_is_empty(sb) ? "true" : "false");

  sb_free(sb);
  free(str);

  return 0;
}
