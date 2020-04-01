#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief an initial length of a string stored inside string builder
 */
#define SB_STR_FRAG_LENGTH 32

/**
 * string builder stores:
 *  - char *str to keep track of the strings appended
 *  - int length the total length of the strings appended
 *  - int __allocated a "private" variable to help to keep track of allocated
 * memory
 */
typedef struct {
  char *str;
  int length;
  int __allocated;

} string_builder_t;

/**
 * Initialize the string_builder_t structure
 *
 * @return a string_builder_t* pointer with initialized values
 */
string_builder_t *sb_init();

/**
 * Check if the concents of string builder are empty or not
 *
 * @param sb a pointer to the initialized string_builder_t struct
 *
 * @return true, if string builder is empty
 */
bool sb_is_empty(string_builder_t *sb);

/**
 * Append a char* string to a string builder instance
 *
 * @param sb a pointer to the initialized string_builder_t struct
 * @param str a string that will be appended
 *
 * @return true, if str was appended
 */
bool sb_append(string_builder_t *sb, const char *str);

/**
 * Build the string, that stored in the string builder
 *
 * @param sb a pointer to the initialized string_builder_t struct
 *
 * @return a string that is stored in string builder
 */
char *sb_build_str(string_builder_t *sb);

/**
 * Reset instance of the string builder.
 * The function will clear the string stored and reset other variables to
 * initial state.
 *
 * @param sb a pointer to the initialized string_builder_t struct
 */
void sb_reset(string_builder_t *sb);

/**
 * Empty the memory for given instance of string_builder_t
 *
 * @param sb a pointer to the initialized string_builder_t struct
 */
void sb_free(string_builder_t *sb);

#endif /* STRING_BUILDER_H */
