#include "./include/string_builder.h"

/**
 * Initialize the string_builder_t structure
 *
 * @return a string_builder_t* pointer with initialized values
 */
string_builder_t *sb_init() {
  string_builder_t *sb =
      (string_builder_t *)calloc(1, sizeof(string_builder_t));

  sb->length = 0;
  sb->str = (char *)malloc(sizeof(char) * SB_STR_FRAG_LENGTH);
  sb->__allocated = sizeof(char) * SB_STR_FRAG_LENGTH;
  *sb->str = '\0';

  return sb;
}

/**
 * Check if the concents of string builder are empty or not
 *
 * @param sb a pointer to the initialized string_builder_t struct
 *
 * @return true, if string builder is empty
 */
bool sb_is_empty(string_builder_t *sb) {
  return sb->length == 0 || sb->str == NULL;
}

/**
 * Append a char* string to a string builder instance
 *
 * @param sb a pointer to the initialized string_builder_t struct
 * @param str a string that will be appended
 *
 * @return true, if str was appended
 */
bool sb_append(string_builder_t *sb, const char *str) {
  if (str == NULL) {
    return false;
  }

  int len = strlen(str);

  // Add space for a string, if needed
  while (sb->__allocated < sb->length + len + 1) {
    // increate the allocated size by 2 to be able to insert more strings
    // (not the most efficient solution)
    sb->__allocated *= 2;
  }

  // reallocate memory for the string stored in the string builder
  sb->str = (char *)realloc(sb->str, sb->__allocated);

  // move the memory to new reallocated memory
  memmove(sb->str + sb->length, str, len);

  // update string builder variables
  sb->length += len;
  sb->str[sb->length] = '\0';

  return true;
}

/**
 * Build the string, that stored in the string builder
 *
 * @param sb a pointer to the initialized string_builder_t struct
 *
 * @return a string that is stored in string builder
 */
char *sb_build_str(string_builder_t *sb) { return sb->str; }

/**
 * Reset instance of the string builder.
 * The function will clear the string stored and reset other variables to
 * initial state.
 *
 * @param sb a pointer to the initialized string_builder_t struct
 */
void sb_reset(string_builder_t *sb) {
  sb->str = NULL;
  sb->length = 0;
}

/**
 * Empty the memory for given instance of string_builder_t
 *
 * @param sb a pointer to the initialized string_builder_t struct
 */
void sb_free(string_builder_t *sb) {
  free(sb->str);
  free(sb);
}
