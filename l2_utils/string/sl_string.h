/***********************************************************************************************************************
 * SIBROS TECHNOLOGIES, INC. CONFIDENTIAL
 * Copyright (c) 2018 - 2020 Sibros Technologies, Inc.
 * All Rights Reserved.
 * NOTICE: All information contained herein is, and remains the property of Sibros Technologies, Inc. and its suppliers,
 * if any. The intellectual and technical concepts contained herein are proprietary to Sibros Technologies, Inc. and its
 * suppliers and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or
 * copyright law. Dissemination of this information or reproduction of this material is strictly forbidden unless prior
 * written permission is obtained from Sibros Technologies, Inc.
 **********************************************************************************************************************/

/**
 * @file
 * Library for generic string implementations
 *
 * The library is based on static memory allocation and the string memory will not allocate more
 * memory or grow after initialization.
 *
 * @code
 *   char memory[64];
 *   sl_string_s string = sl_string__initialize(memory, sizeof(memory));
 *
 *   sl_string__set(string, "ab");
 *   assert(sl_string__equals_to(string, "ab");
 * @endcode
 *
 * Thread Safety Assessment:
 * This library is not thread safe if the same string is used by multiple tasks.
 * Multiple strings used by different tasks is thread safe.
 */
#ifndef SIBROS__SL_STRING_H
#define SIBROS__SL_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */
#include <ctype.h>
#include <stdarg.h> //lint !e829 <stdarg> is required for our desired functionality
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* External Includes */

/* Module Includes */

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/

static const int sl_string_error = -1;

/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

typedef struct {
  char *cstring;
  size_t max_size;
} sl_string_s;

/***********************************************************************************************************************
 *
 *                                     F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

/**
 * The minimum size required for a string is 4. This accounts for the
 * two bytes required for the header and one byte required for the null term.
 */
sl_string_s sl_string__initialize(void *static_memory, size_t static_memory_size_max);

/**
 * Same as sl_string__initialize() but the string will be copied from 'copy_from'
 */
sl_string_s sl_string__initialize_from(void *static_memory, size_t static_memory_size_max, const char *copy_from);

/**
 * Initialize as non-modifiable, constant string.
 *
 * This is useful such that we can use non-modifiable APIs with 'const sl_string_s'
 * such as sl_string__ends_with()
 *
 * @warning existing_string should be pointer of persistent memory region
 */
sl_string_s sl_string__initialize_non_modifiable(const void *existing_string);

/// @returns true if the string was initialized correctly with non NULL memory
bool sl_string__is_valid(const sl_string_s input_string);

/**
 * @returns the start of the string, similar to std::string.c_str()
 * @note This returns const pointer that should not be modified
 *
 * @code
 * printf("%s %s", s, sl_string__c_str(&s));
 * @endcode
 */
const char *sl_string__c_str(const sl_string_s input_string);

/**
 * Similar to sl_string__c_str() but this API is to express the intent that the returned cstring pointer
 * is to be modfied. Care should be taken to only modify contents based on sl_string__get_capacity()
 */
char *sl_string__get_modifiable_c_str(const sl_string_s input_string);

/**
 *
 * @returns the capacity of the string excluding the string header
 * @note the capacity is how many chars we can hold excluding the NULL char
 */
size_t sl_string__get_capacity(const sl_string_s input_string);

/**
 * Similar to sl_string__get_capacity() but should return +1
 */
size_t sl_string__get_capacity_including_null_char(const sl_string_s input_string);

/**
 * @returns the current length of the string
 */
size_t sl_string__get_length(const sl_string_s input_string);

/**
 * @returns true if the string has reached its memory capacity
 * @note this will return true if string was constructed through sl_string__initialize_non_modifiable()
 */
bool sl_string__is_full(const sl_string_s input_string);

/**
 * Converts a string to integer value
 * The input_string can be a hex number such as:
 *  - 0x123
 *  - 12345 (decimal, base 10)
 */
long int sl_string__to_int(const sl_string_s input_string);

double sl_string__to_float(const sl_string_s input_string);

bool sl_string__is_alpha(const sl_string_s input_string);
bool sl_string__is_alphanum(const sl_string_s input_string);

bool sl_string__clear(sl_string_s input_string);
bool sl_string__clear_all_memory(sl_string_s input_string);

bool sl_string__to_lower(sl_string_s input_string);
bool sl_string__to_upper(sl_string_s input_string);

/**
 * @{
 * @name printf API
 *
 * @note
 * The 'n' style API, namely snprintf() is not provided because all sl_string printf() style API is bounds protected
 */
size_t sl_string__printf(sl_string_s input_string, const char *format, ...) __attribute__((format(printf, 2, 3)));
size_t sl_string__vprintf(sl_string_s input_string, const char *format, va_list args);

size_t sl_string__printf_append(sl_string_s input_string, const char *format, ...)
    __attribute__((format(printf, 2, 3)));
size_t sl_string__vprintf_append(sl_string_s input_string, const char *format, va_list args);
/** @} */

/**
 * Copy the string to the user provided pointer with max size of 'copy_to_max_size' including NULL
 * @returns the number of bytes copied not including null terminator
 * @returns If the 'copy_to' is a NULL pointer, then this returns the number of bytes that would have been copied
 */
size_t sl_string__copy_to(const sl_string_s input_string, char *copy_to, size_t copy_to_max_size);

size_t sl_string__scanf(const sl_string_s input_string, const char *format, ...) __attribute__((format(scanf, 2, 3)));

/**
 * Perform string tokenization (original copy is destroyed)
 * If you want to get pointers separating the stringing such as "hello world 123",
 * then you can use this function, however, your original stringing will be destroyed.
 * @code
 * char string_memory[100];
 * sl_string_s s = sl_string__initialize(string_memory, sizeof(string_memory));
 * sl_string__append(s, "123 45 6789");
 *
 * char *one, *two, *three;
 * if (sl_string__tokenize(s, " ", 3, &one, &two, &three) == 3) {
 *     printf("%s %s %s\n", one, two, three);
 * }
 * @endcode
 */
int sl_string__tokenize(sl_string_s input_string, const char *delimiter_string, int char_ptr_count, ...);

bool sl_string__set(sl_string_s input_string, const char *source);
bool sl_string__insert_at(sl_string_s input_string, size_t index_position, const char *insert);
bool sl_string__append(sl_string_s input_string, const char *append);
bool sl_string__append_char(sl_string_s input_string, char append);

/**
 * Append a string to sl_string_s string. If the string to be appended is larger than the remaining capacity, the string
 * to be appended will be truncated to fit in the remaining space of sl_string_s string.
 * @returns the number of bytes copied not including null terminator
 */
size_t sl_string__append_truncate(sl_string_s input_string, const char *append);

bool sl_string__equals_to(const sl_string_s input_string, const char *compare_with);
bool sl_string__equals_to_ignore_case(const sl_string_s input_string, const char *compare_with_case_ignored);

int sl_string__last_index_of(const sl_string_s input_string, const char *index_of);
int sl_string__last_index_of_ignore_case(const sl_string_s input_string, const char *index_of_case_ignored);
int sl_string__first_index_of(const sl_string_s input_string, const char *index_of);
int sl_string__first_index_of_ignore_case(const sl_string_s input_string, const char *index_of_case_ignored);

bool sl_string__contains(const sl_string_s input_string, const char *substring);
bool sl_string__contains_ignore_case(const sl_string_s input_string, const char *substring_case_ignored);

int sl_string__count_of(const sl_string_s input_string, const char *count_of);

/**
 * @returns true if string begins with 'begin_with' but will compare only max_chars_of_begin_with_to_compare
 */
bool sl_string__begins_with_limited_chars(const sl_string_s input_string, const char *begin_with,
                                          size_t max_chars_of_begin_with_to_compare);
bool sl_string__begins_with(const sl_string_s input_string, const char *begin_with);
bool sl_string__begins_with_ignore_case(const sl_string_s input_string, const char *begin_with_case_ignored);

bool sl_string__begins_with_whole_word(const sl_string_s input_string, const char *begin_with,
                                       const char *delimiter_string);
bool sl_string__begins_with_whole_word_ignore_case(const sl_string_s input_string, const char *begin_with_case_ignored,
                                                   const char *delimiter_string);

bool sl_string__ends_with(const sl_string_s input_string, const char *ends_with);
bool sl_string__ends_with_ignore_case(const sl_string_s input_string, const char *ends_with_case_ignored);
bool sl_string__ends_with_newline(const sl_string_s input_string);

bool sl_string__erase(sl_string_s input_string, const char *erase);
bool sl_string__erase_first(sl_string_s input_string, size_t n_chars);
bool sl_string__erase_last(sl_string_s input_string, size_t n_chars);
bool sl_string__erase_at(sl_string_s input_string, size_t index_position);
bool sl_string__erase_after(sl_string_s input_string, size_t erase_index_position, size_t n_chars);
bool sl_string__erase_at_substring(sl_string_s input_string, const char *erase_at_substring);
bool sl_string__erase_first_word(sl_string_s input_string, char word_separater);
size_t sl_string__erase_special_chars(sl_string_s input_string);

/**
 * Erases or parses integer only from the string
 * For example, if we have string = ";123 abc 456" then:
 *  - First call to this function will set *erased_int to 123, and string will be " abc 1234"
 *  - Second call to this function will set *erased_int to 456, and set the string to ""
 */
bool sl_string__erase_int(sl_string_s input_string, long int *erased_int);

bool sl_string__trim_end(sl_string_s input_string, const char *chars_to_trim);
bool sl_string__trim_start(sl_string_s input_string, const char *chars_to_trim);

/**
 * @{
 * @name This comment applies for all sl_string_replace_first/last functions.
 *
 * @returns True if was able to successfully replace or if it can not find `replace`, otherwise false
 *
 * The use case for these functions are: I want to replace first/last/all of the occurrences of `replace` with
 * `replace_with`. If replace doesn't exist you've still accomplished this. Thus the function will return true
 * even if it can not find `replace` in `string`.
 */
bool sl_string__replace_first(sl_string_s input_string, const char *replace, const char *replace_with);
bool sl_string__replace_last(sl_string_s input_string, const char *replace, const char *replace_with);
/** @} */

/**
 * @returns number of substrings successfully replaced.
 * If a replace attempt fails this function will return -1.
 * All replace attempts before failure will still apply.
 */
int sl_string__replace_all(sl_string_s input_string, const char *replace, const char *replace_with);

bool sl_string__contains_only_hex_number(const sl_string_s input_string);
bool sl_string__contains_only_decimal_number(const sl_string_s input_string);

/**
 * checks whether input strings is a number or not.
 *
 * @returns True if string is either decimal or hexadecimal number
 */
bool sl_string__contains_only_number(const sl_string_s input_string);

/**
 * Search for `match_string` in `whole_string` up to either a NULL-terminating character or `length_to_search` number of
 * characters
 *
 * The behavior is similar to `strstr()`, but with an additional length limit of `length_to_search` to search. See
 * https://www.freebsd.org/cgi/man.cgi?query=strnstr&sektion=3 for more detail on expected behavior. NULL pointer
 * parameters are also checked.
 *
 * @return Pointer to beginning of `match_string` match in `whole_string` if found; NULL otherwise
 */
char *sl_string__strnstr(const sl_string_s whole_string, const char *match_string, size_t length_to_search);

#ifdef __cplusplus
} /* extern "c" */
#endif
#endif /* #ifndef SIBROS__SL_STRING_H */
