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

/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Main Module Header */
#include "sl_string.h"

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* External Includes */

// lint -efile(901, *sl_string.c) args not initialized; va_list should never be initialized
// lint -efile(953, *sl_string.c) args could be const, va_list doesn't accept constants

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/
#ifndef NOOP
#define NOOP(x)
#endif

/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                             P R I V A T E   F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                  P R I V A T E   D A T A   D E F I N I T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                         P R I V A T E   F U N C T I O N S
 *
 **********************************************************************************************************************/

ptrdiff_t sl_utils__pointer_distance(const void *first, const void *second) {
  ptrdiff_t distance = 0;
  if ((NULL != first) && (NULL != second)) {
    distance = (intptr_t)first - (intptr_t)second;
  }
  return distance;
}

static bool sl_string__private_can_be_modified(const sl_string_s input_string) { return (input_string.max_size > 0U); }

static size_t sl_string__private_return_positive_or_zero(const int value) {
  const size_t value_size = (size_t)(unsigned)value;
  return (value > 0) ? value_size : 0U;
}

static bool sl_string__private_ensure_enough_memory_for_insertion(const sl_string_s input_string,
                                                                  const char *insertion_string) {
  const size_t length_of_original_string = sl_string__get_length(input_string);

  const size_t length_of_insertion_string = strlen(insertion_string);
  const size_t insertion_memory_required = length_of_original_string + length_of_insertion_string;
  const bool is_memory_enough = (insertion_memory_required <= sl_string__get_capacity(input_string));

  return is_memory_enough;
}

/**
 * Some compilers do not offer strtok_r() hence we had to port this over
 */
static char *sl_string__private_strtok_r(char *input_string, const char *delimiter_string, char **save_ptr) {
  char *end_of_token = NULL;
  char *active_token = NULL;
  char *seek_string = NULL;

  if (NULL == input_string) {
    seek_string = *save_ptr;
  } else {
    seek_string = input_string;
  }

  if (0U != strlen(seek_string)) {
    seek_string += strspn(seek_string, delimiter_string);
    *save_ptr = seek_string;
  }

  if (0U != strlen(seek_string)) {
    end_of_token = seek_string + strcspn(seek_string, delimiter_string);
    *save_ptr = end_of_token;
    active_token = seek_string;

    if ('\0' != *end_of_token) {
      *end_of_token = '\0';
      *save_ptr = end_of_token + 1;
      active_token = seek_string;
    }
  }

  return active_token;
}

/***********************************************************************************************************************
 *
 *                                          P U B L I C   F U N C T I O N S
 *
 **********************************************************************************************************************/

sl_string_s sl_string__initialize(void *static_memory, size_t static_memory_size_max) {
  sl_string_s output_string = {NULL};

  if ((NULL != static_memory) && (static_memory_size_max > 0U)) {
    memset(static_memory, 0, static_memory_size_max);
    output_string.cstring = static_memory;
    output_string.max_size = static_memory_size_max;
  }

  return output_string;
}

sl_string_s sl_string__initialize_from(void *static_memory, size_t static_memory_size_max, const char *copy_from) {
  sl_string_s output_string = {NULL};

  if ((NULL != static_memory) && (NULL != copy_from)) {
    output_string = sl_string__initialize(static_memory, static_memory_size_max);

    /* Assume it is okay if we fail to copy the string due to insufficient memory
     * This would be better than returning un-initialized string
     */
    (void)sl_string__set(output_string, copy_from);
  }

  return output_string;
}

sl_string_s sl_string__initialize_non_modifiable(const void *existing_string) {
  /* 'const void*' parameter is used to express to the user that we will never modify this input
   * We cast it here, but never modify the string otherwise because we set 'max_size' to zero
   */
  const sl_string_s output_string = {
      .cstring = (char *)existing_string, // lint !e9005
      .max_size = 0U,
  };
  return output_string;
}

bool sl_string__is_valid(const sl_string_s input_string) { return NULL != input_string.cstring; }

size_t sl_string__get_capacity_including_null_char(const sl_string_s input_string) {
  size_t capacity = 0;

  if (sl_string__is_valid(input_string)) {
    capacity = input_string.max_size;
  }

  return capacity;
}

size_t sl_string__get_capacity(const sl_string_s input_string) {
  size_t capacity = sl_string__get_capacity_including_null_char(input_string);

  if (capacity > 0U) {
    --capacity;
  }

  return capacity;
}

size_t sl_string__get_length(const sl_string_s input_string) {
  return sl_string__is_valid(input_string) ? strlen(input_string.cstring) : 0U;
}

bool sl_string__is_full(const sl_string_s input_string) {
  // Note: For non-modifiable string constructed from sl_string__initialize_non_modifiable(), this should return true
  bool is_full = (false == sl_string__private_can_be_modified(input_string));

  if (sl_string__is_valid(input_string)) {
    const size_t length = sl_string__get_length(input_string);
    const size_t capacity = sl_string__get_capacity(input_string);
    is_full = (length >= capacity);
  }
  return is_full;
}

const char *sl_string__c_str(const sl_string_s input_string) { return input_string.cstring; }

char *sl_string__get_modifiable_c_str(const sl_string_s input_string) {
  char *mutable_memory = NULL;

  if (sl_string__private_can_be_modified(input_string)) {
    mutable_memory = input_string.cstring;
  }

  return mutable_memory;
}

long int sl_string__to_int(const sl_string_s input_string) {
  long int value_of_string = 0;
  if (sl_string__is_valid(input_string)) {
    int base = 10;
    if (sl_string__begins_with_ignore_case(input_string, "0x")) {
      base = 16;
    } else {
      base = 10;
    }
    value_of_string = strtol(input_string.cstring, NULL, base);
  }
  return value_of_string;
}

double sl_string__to_float(const sl_string_s input_string) {
  return sl_string__is_valid(input_string) ? strtof(input_string.cstring, NULL) : 0.0f;
}

bool sl_string__is_alpha(const sl_string_s input_string) {
  bool is_alpha = false;
  if (sl_string__is_valid(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    is_alpha = true;
    for (size_t character = 0; character < input_string_length; ++character) {
      if (0U == isalpha((int)input_string.cstring[character])) {
        is_alpha = false;
        break;
      }
    }
  }
  return is_alpha;
}

bool sl_string__is_alphanum(const sl_string_s input_string) {
  bool is_alpha_num = false;
  if (sl_string__is_valid(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    is_alpha_num = true;
    for (size_t character = 0; character < input_string_length; ++character) {
      if (0U == isalnum((int)input_string.cstring[character])) {
        is_alpha_num = false;
        break;
      }
    }
  }
  return is_alpha_num;
}

bool sl_string__clear(sl_string_s input_string) {
  bool clear_success = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    *input_string.cstring = '\0';
    clear_success = true;
  }
  return clear_success;
}

bool sl_string__clear_all_memory(sl_string_s input_string) {
  bool clear_success = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    memset(input_string.cstring, 0, sl_string__get_capacity_including_null_char(input_string));
    clear_success = true;
  }
  return clear_success;
}

bool sl_string__to_lower(sl_string_s input_string) {
  bool conversion_done = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    char *iteration_string = input_string.cstring;
    while ('\0' != *iteration_string) {
      *iteration_string = (char)tolower((int)(*iteration_string));
      ++iteration_string;
    }
    conversion_done = true;
  }
  return conversion_done;
}

bool sl_string__to_upper(sl_string_s input_string) {
  bool conversion_done = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    char *iterator = input_string.cstring;
    while ('\0' != *iterator) {
      *iterator = (char)toupper((int)(*iterator));
      ++iterator;
    }
    conversion_done = true;
  }
  return conversion_done;
}

size_t sl_string__printf(sl_string_s input_string, const char *format, ...) {
  size_t chars_printed = 0;
  if (sl_string__is_valid(input_string) && (NULL != format) && sl_string__private_can_be_modified(input_string)) {
    va_list args;
    va_start(args, format);
    chars_printed = sl_string__vprintf(input_string, format, args);
    va_end(args);
  }
  return chars_printed;
}

size_t sl_string__vprintf(sl_string_s input_string, const char *format, va_list args) {
  size_t chars_printed_val = 0;
  if (sl_string__is_valid(input_string) && (NULL != format) && sl_string__private_can_be_modified(input_string)) {
    const size_t capacity = sl_string__get_capacity_including_null_char(input_string);
    const int chars_printed = vsnprintf(input_string.cstring, capacity, format, args);

    chars_printed_val = sl_string__private_return_positive_or_zero(chars_printed);
  }
  return chars_printed_val;
}

size_t sl_string__printf_append(sl_string_s input_string, const char *format, ...) {
  size_t chars_printed = 0;
  if (sl_string__is_valid(input_string) && (NULL != format) && sl_string__private_can_be_modified(input_string)) {
    va_list args;
    va_start(args, format);
    chars_printed = sl_string__vprintf_append(input_string, format, args);
    va_end(args);
  }
  return chars_printed;
}

size_t sl_string__copy_to(const sl_string_s input_string, char *copy_to, size_t copy_to_max_size) {
  size_t bytes_to_copy = 0;
  if (sl_string__is_valid(input_string)) {
    const size_t len = sl_string__get_length(input_string);
    const size_t bytes_to_copy_excluding_null = (copy_to_max_size > 0U) ? (copy_to_max_size - 1U) : 0U;
    bytes_to_copy = (len < bytes_to_copy_excluding_null) ? len : bytes_to_copy_excluding_null;

    if (NULL != copy_to) {
      (void)memcpy(copy_to, input_string.cstring, bytes_to_copy);
      copy_to[bytes_to_copy] = '\0';
    }
  }
  return bytes_to_copy;
}

size_t sl_string__vprintf_append(sl_string_s input_string, const char *format, va_list args) {
  size_t chars_printed_val = 0;

  if (sl_string__is_valid(input_string) && (NULL != format) && sl_string__private_can_be_modified(input_string)) {
    const size_t existing_length = sl_string__get_length(input_string);
    const size_t remaining_capacity = (sl_string__get_capacity_including_null_char(input_string) - existing_length);
    const int chars_printed = vsnprintf(input_string.cstring + existing_length, remaining_capacity, format, args);
    chars_printed_val = sl_string__private_return_positive_or_zero(chars_printed);
  }

  return chars_printed_val;
}

size_t sl_string__scanf(const sl_string_s input_string, const char *format, ...) {
  int number_of_parsed_params = 0;

  if (sl_string__is_valid(input_string) && (NULL != format)) {
    va_list args;
    va_start(args, format);
    // lint -esym(586, vsscanf) vsscanf is deprecated
    number_of_parsed_params = vsscanf(input_string.cstring, format, args);
    va_end(args);
  }

  return sl_string__private_return_positive_or_zero(number_of_parsed_params);
}

int sl_string__tokenize(sl_string_s input_string, const char *delimiter_string, int char_ptr_count, ...) {
  int count_of_token = sl_string_error;

  if (sl_string__is_valid(input_string) && (NULL != delimiter_string)) {
    if (sl_string__private_can_be_modified(input_string)) {
      va_list args;
      va_start(args, char_ptr_count);

      count_of_token = 0;
      char *save_pointer = NULL;
      char *active_token = sl_string__private_strtok_r(input_string.cstring, delimiter_string, &save_pointer);

      int char_ptr_count_remaining = char_ptr_count;
      while ((NULL != active_token) && (char_ptr_count_remaining > 0)) {
        char **const param_argument = va_arg(args, char **);
        if (NULL != param_argument) {
          *param_argument = active_token;
        }

        active_token = sl_string__private_strtok_r(NULL, delimiter_string, &save_pointer);

        --char_ptr_count_remaining;
        ++count_of_token;
      }
      va_end(args);
    }
  }

  return count_of_token;
}

bool sl_string__set(sl_string_s input_string, const char *source) {
  bool result = false;

  if ((NULL != input_string.cstring) && (NULL != source) && sl_string__private_can_be_modified(input_string)) {
    const size_t source_length = strlen(source);
    const size_t destination_length = sl_string__get_capacity(input_string);
    if (destination_length >= source_length) {
      strncpy(input_string.cstring, source, destination_length);
      result = true;
    }
  }

  return result;
}

bool sl_string__insert_at(sl_string_s input_string, size_t index_position, const char *insert) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != insert) && sl_string__private_can_be_modified(input_string)) {
    if (index_position <= sl_string__get_length(input_string)) {
      if (sl_string__private_ensure_enough_memory_for_insertion(input_string, insert)) {
        const size_t length_of_insertion_string = strlen(insert);

        char *const position_of_insertion = input_string.cstring + index_position;
        char *const destination_of_substring = position_of_insertion + length_of_insertion_string;
        // Plus 1 for the null term

        const size_t length_to_be_moved = (sl_string__get_length(input_string) - index_position) + 1U;
        (void)memmove(destination_of_substring, position_of_insertion, length_to_be_moved);
        strncpy(position_of_insertion, insert, length_of_insertion_string);

        result = true;
      }
    }
  }
  return result;
}

bool sl_string__append(sl_string_s input_string, const char *append) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != append) && sl_string__private_can_be_modified(input_string)) {
    const size_t length_of_original_string = sl_string__get_length(input_string);
    const size_t length_of_insertion_string = strlen(append);

    if ((length_of_original_string + length_of_insertion_string) <= sl_string__get_capacity(input_string)) {
      const size_t length_to_copy_including_null = (1U + length_of_insertion_string);
      (void)memcpy((input_string.cstring + length_of_original_string), append, length_to_copy_including_null);
      result = true;
    }
  }
  return result;
}

bool sl_string__append_char(sl_string_s input_string, char append) {
  bool result = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    const char append_string[] = {append, '\0'};
    result = sl_string__append(input_string, append_string);
  }
  return result;
}

size_t sl_string__append_truncate(sl_string_s input_string, const char *append) {
  size_t appended_chars_count = 0U;
  if (sl_string__is_valid(input_string) && (NULL != append) && sl_string__private_can_be_modified(input_string)) {
    const size_t length_of_insertion_string_without_null = strlen(append);
    const size_t length_of_original_string = sl_string__get_length(input_string);
    const size_t remaining_capacity = (sl_string__get_capacity(input_string) - length_of_original_string);
    const size_t number_of_chars_to_append = (length_of_insertion_string_without_null < remaining_capacity)
                                                 ? length_of_insertion_string_without_null
                                                 : remaining_capacity;
    // lint -esym(586, strncat) strncat is deprecated
    (void)strncat((input_string.cstring + length_of_original_string), append, number_of_chars_to_append);
    appended_chars_count = number_of_chars_to_append;
  }
  return appended_chars_count;
}

bool sl_string__equals_to(const sl_string_s input_string, const char *compare_with) {
  bool equal = false;

  if (sl_string__is_valid(input_string) && (NULL != compare_with)) {
    equal = (0 == strcmp(input_string.cstring, compare_with));
  }
  return equal;
}

bool sl_string__equals_to_ignore_case(const sl_string_s input_string, const char *compare_with_case_ignored) {
  bool equal = false;

  if (sl_string__is_valid(input_string) && (NULL != compare_with_case_ignored)) {
    equal = (0 == strcasecmp(input_string.cstring, compare_with_case_ignored));
  }
  return equal;
}

int sl_string__last_index_of(const sl_string_s input_string, const char *index_of) {
  const char *pointer_to_last = NULL;

  if (sl_string__is_valid(input_string) && (NULL != index_of)) {
    const char *pointer_to_first_in_substring = strstr(input_string.cstring, index_of);
    const size_t substring_length = strlen(index_of);

    while (NULL != pointer_to_first_in_substring) {
      pointer_to_last = pointer_to_first_in_substring;
      pointer_to_first_in_substring = strstr(pointer_to_first_in_substring + substring_length, index_of);
    }
  }
  return ((NULL != pointer_to_last) ? (int)sl_utils__pointer_distance(pointer_to_last, input_string.cstring)
                                    : sl_string_error);
}

int sl_string__last_index_of_ignore_case(const sl_string_s input_string, const char *index_of_case_ignored) {
  int index_of_string = sl_string_error;

  if (sl_string__is_valid(input_string) && (NULL != index_of_case_ignored)) {
    const char *pointer_to_first_char_in_substring =
        strchr(input_string.cstring, tolower((int)(*index_of_case_ignored)));
    if (NULL == pointer_to_first_char_in_substring) {
      pointer_to_first_char_in_substring = strchr(input_string.cstring, toupper((int)(*index_of_case_ignored)));
    }

    const size_t length_of_string_to_find = strlen(index_of_case_ignored);
    while (NULL != pointer_to_first_char_in_substring) {
      size_t num_of_chars_to_move_substring_start_location = 1;

      if (0 == strncasecmp(pointer_to_first_char_in_substring, index_of_case_ignored, length_of_string_to_find)) {
        index_of_string = (int)sl_utils__pointer_distance(pointer_to_first_char_in_substring, input_string.cstring);
        num_of_chars_to_move_substring_start_location = length_of_string_to_find;
      }

      const char *const pointer_to_last_char = pointer_to_first_char_in_substring;
      pointer_to_first_char_in_substring = strchr(pointer_to_last_char + num_of_chars_to_move_substring_start_location,
                                                  tolower((int)(*index_of_case_ignored)));

      if (NULL == pointer_to_first_char_in_substring) {
        pointer_to_first_char_in_substring =
            strchr(pointer_to_last_char + num_of_chars_to_move_substring_start_location,
                   toupper((int)(*index_of_case_ignored)));
      }
    }
  }
  return index_of_string;
}

int sl_string__first_index_of(const sl_string_s input_string, const char *index_of) {
  const char *pointer_to_first = NULL;

  if (sl_string__is_valid(input_string) && (NULL != index_of)) {
    pointer_to_first = strstr(input_string.cstring, index_of);
  }

  return ((NULL != pointer_to_first) ? (int)sl_utils__pointer_distance(pointer_to_first, input_string.cstring)
                                     : sl_string_error);
}

int sl_string__first_index_of_ignore_case(const sl_string_s input_string, const char *index_of_case_ignored) {
  int index_of_string = sl_string_error;

  if (sl_string__is_valid(input_string) && (NULL != index_of_case_ignored)) {
    const char *pointer_to_first_char_in_substring =
        strchr(input_string.cstring, tolower((int)(*index_of_case_ignored)));
    if (NULL == pointer_to_first_char_in_substring) {
      pointer_to_first_char_in_substring = strchr(input_string.cstring, toupper((int)(*index_of_case_ignored)));
    }

    const size_t length_of_index_of_case_ignored = strlen(index_of_case_ignored);
    while (NULL != pointer_to_first_char_in_substring) {
      const size_t num_of_chars_to_move_substring_start_location = 1;

      if (0 ==
          strncasecmp(pointer_to_first_char_in_substring, index_of_case_ignored, length_of_index_of_case_ignored)) {
        index_of_string = (int)sl_utils__pointer_distance(pointer_to_first_char_in_substring, input_string.cstring);
        break;
      }

      const char *const first_char_in_substring_storage_ptr = pointer_to_first_char_in_substring;
      pointer_to_first_char_in_substring =
          strchr(first_char_in_substring_storage_ptr + num_of_chars_to_move_substring_start_location,
                 tolower((int)(*index_of_case_ignored)));

      if (NULL == pointer_to_first_char_in_substring) {
        pointer_to_first_char_in_substring =
            strchr(first_char_in_substring_storage_ptr + num_of_chars_to_move_substring_start_location,
                   toupper((int)(*index_of_case_ignored)));
      }
    }
  }
  return index_of_string;
}

bool sl_string__contains(const sl_string_s input_string, const char *substring) {
  int contains = sl_string_error;
  if (sl_string__is_valid(input_string) && (NULL != substring)) {
    contains = sl_string__first_index_of(input_string, substring);
  }
  return (sl_string_error != contains);
}

bool sl_string__contains_ignore_case(const sl_string_s input_string, const char *substring_case_ignored) {
  int contains = sl_string_error;
  if (sl_string__is_valid(input_string) && (NULL != substring_case_ignored)) {
    contains = sl_string__first_index_of_ignore_case(input_string, substring_case_ignored);
  }
  return (sl_string_error != contains);
}

int sl_string__count_of(const sl_string_s input_string, const char *count_of) {
  int count = sl_string_error;

  if (sl_string__is_valid(input_string) && (NULL != count_of)) {
    count = 0;
    const char *pointer_to_first_in_substring = strstr(input_string.cstring, count_of);
    const size_t length_of_count_of_string = strlen(count_of);
    while (NULL != pointer_to_first_in_substring) {
      ++count;
      pointer_to_first_in_substring = strstr(pointer_to_first_in_substring + length_of_count_of_string, count_of);
    }
  }

  return count;
}

bool sl_string__begins_with(const sl_string_s input_string, const char *begin_with) {
  bool begins_with = false;
  if (sl_string__is_valid(input_string) && (NULL != begin_with)) {
    const size_t substring_length = strlen(begin_with);
    begins_with = sl_string__begins_with_limited_chars(input_string, begin_with, substring_length);
  }
  return begins_with;
}

bool sl_string__begins_with_limited_chars(const sl_string_s input_string, const char *begin_with,
                                          size_t max_chars_of_begin_with_to_compare) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != begin_with)) {
    const size_t input_string_length = sl_string__get_length(input_string);

    // strncmp/strncasecmp are used here instead of strstr as it
    // allows to check only the beginning of the string
    if (input_string_length >= max_chars_of_begin_with_to_compare) {
      if (0 == strncmp(input_string.cstring, begin_with, max_chars_of_begin_with_to_compare)) {
        result = true;
      }
    }
  }
  return result;
}

bool sl_string__begins_with_ignore_case(const sl_string_s input_string, const char *begin_with_case_ignored) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != begin_with_case_ignored)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t substring_length = strlen(begin_with_case_ignored);

    // strncmp/strncasecmp are used here instead of strstr as it
    // allows to check only the beginning of the string
    if (input_string_length >= substring_length) {
      if (0 == strncasecmp(input_string.cstring, begin_with_case_ignored, substring_length)) {
        result = true;
      }
    }
  }
  return result;
}

bool sl_string__begins_with_whole_word(const sl_string_s input_string, const char *begin_with,
                                       const char *delimiter_string) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != begin_with) && (NULL != delimiter_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t substring_length = strlen(begin_with);

    // strncmp/strncasecmp are used here instead of strstr as it
    // allows to check only the beginning of the string
    if (input_string_length >= substring_length) {
      if (0 == strncmp(input_string.cstring, begin_with, substring_length)) {
        result = ((*delimiter_string == input_string.cstring[substring_length]) ||
                  ('\0' == input_string.cstring[substring_length]));
      }
    }
  }
  return result;
}

bool sl_string__begins_with_whole_word_ignore_case(const sl_string_s input_string, const char *begin_with_case_ignored,
                                                   const char *delimiter_string) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != begin_with_case_ignored) && (NULL != delimiter_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t substring_length = strlen(begin_with_case_ignored);

    if (input_string_length >= substring_length) {
      if (0 == strncasecmp(input_string.cstring, begin_with_case_ignored, substring_length)) {
        result = ((*delimiter_string == input_string.cstring[substring_length]) || ('\0' == *delimiter_string));
      }
    }
  }
  return result;
}

bool sl_string__ends_with(const sl_string_s input_string, const char *ends_with) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != ends_with)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t substring_length = strlen(ends_with);

    if (input_string_length >= substring_length) {
      if (0 == strncmp(input_string.cstring + input_string_length - substring_length, ends_with, substring_length)) {
        result = true;
      }
    }
  }
  return result;
}

bool sl_string__ends_with_ignore_case(const sl_string_s input_string, const char *ends_with_case_ignored) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != ends_with_case_ignored)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t substring_length = strlen(ends_with_case_ignored);

    if (input_string_length >= substring_length) {
      if (0 == strncasecmp(input_string.cstring + input_string_length - substring_length, ends_with_case_ignored,
                           substring_length)) {
        result = true;
      }
    }
  }
  return result;
}

bool sl_string__ends_with_newline(const sl_string_s input_string) {
  bool status = false;
  if (sl_string__is_valid(input_string)) {
    const size_t length = sl_string__get_length(input_string);

    if (length > 0U) {
      const size_t last_index = length - 1U;
      const char last_char = input_string.cstring[last_index];
      status = ('\r' == last_char) || ('\n' == last_char);
    }
  }
  return status;
}

bool sl_string__erase(sl_string_s input_string, const char *erase) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != erase) && sl_string__private_can_be_modified(input_string)) {
    const int index_to_erase = sl_string__first_index_of(input_string, erase);
    if (sl_string_error != index_to_erase) {
      result = sl_string__erase_after(input_string, (size_t)(unsigned)index_to_erase, strlen(erase));
    }
  }
  return result;
}

bool sl_string__erase_first(sl_string_s input_string, size_t n_chars) {
  bool result = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    result = sl_string__erase_after(input_string, (size_t)0U, n_chars);
  }
  return result;
}

bool sl_string__erase_last(sl_string_s input_string, size_t n_chars) {
  bool result = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t start_of_erase_index = (n_chars > input_string_length) ? 0U : (input_string_length - n_chars);
    result = sl_string__erase_after(input_string, start_of_erase_index, n_chars);
  }
  return result;
}

bool sl_string__erase_at(sl_string_s input_string, size_t index_position) {
  bool result = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    result = sl_string__erase_after(input_string, index_position, (size_t)1U);
  }
  return result;
}

bool sl_string__erase_after(sl_string_s input_string, size_t erase_index_position, size_t n_chars) {
  bool result = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    size_t n_chars_remaining = n_chars;

    if (erase_index_position < input_string_length) {
      if (n_chars_remaining > (input_string_length - erase_index_position)) {
        n_chars_remaining = (input_string_length - erase_index_position);
      }
      const size_t num_chars_to_move = (size_t)((input_string_length - erase_index_position) - n_chars_remaining) + 1U;
      (void)memmove(input_string.cstring + erase_index_position,
                    input_string.cstring + erase_index_position + n_chars_remaining, num_chars_to_move);
      result = true;
    }
  }
  return result;
}

bool sl_string__erase_at_substring(sl_string_s input_string, const char *erase_at_substring) {
  bool found_something_to_erase = false;

  if (sl_string__is_valid(input_string) && (NULL != erase_at_substring) &&
      sl_string__private_can_be_modified(input_string)) {
    const char *const found = strstr(input_string.cstring, erase_at_substring);
    found_something_to_erase = (NULL != found);

    if (found_something_to_erase) {
      const size_t position = (size_t)sl_utils__pointer_distance(found, input_string.cstring);
      input_string.cstring[position] = '\0';
    }
  }
  return found_something_to_erase;
}

bool sl_string__erase_first_word(sl_string_s input_string, char word_separater) {
  bool result = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    const char *const word_ptr = strchr(input_string.cstring, (int)word_separater);

    if (NULL != word_ptr) {
      const size_t n_chars = (size_t)sl_utils__pointer_distance(word_ptr, input_string.cstring) + 1U;
      result = sl_string__erase_first(input_string, n_chars); // Plus 1 to also erase the word_seperator
    }
  }
  return result;
}

size_t sl_string__erase_special_chars(sl_string_s input_string) {
  size_t chars_removed = 0;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);

    size_t character = 0;
    while (character < input_string_length) {
      if (0U == isalnum((uint8_t)input_string.cstring[character])) {
        if (!sl_string__erase_at(input_string, character)) {
          break;
        }
        ++chars_removed;
      } else {
        ++character;
      }
    }
  }
  return chars_removed;
}

bool sl_string__erase_int(sl_string_s input_string, long int *erased_int) {
  bool parsed = false;
  if (sl_string__is_valid(input_string) && sl_string__private_can_be_modified(input_string)) {
    const char *c = input_string.cstring;
    while (('\0' != *c) && (0U == isdigit((int)*c))) {
      ++c; // Skip chars until a digit
    }

    if (('\0' != *c) && (NULL != erased_int)) {
      *erased_int = strtol(c, NULL, 10);
      parsed = true;
    }

    while (('\0' != *c) && (0U != isdigit((int)*c))) {
      ++c; // Skip the digits we processed in strtol() above
    }

    // Erase the integer we processed above
    const size_t chars_to_erase = (size_t)sl_utils__pointer_distance(c, input_string.cstring);
    (void)sl_string__erase_first(input_string, chars_to_erase);
  }
  return parsed;
}

bool sl_string__trim_end(sl_string_s input_string, const char *chars_to_trim) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != chars_to_trim) &&
      sl_string__private_can_be_modified(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t trim_char_length = strlen(chars_to_trim);

    for (int character = ((int)input_string_length - 1); character >= 0; --character) {
      bool trim_done = false;

      for (size_t trim_char = 0; trim_char < trim_char_length; ++trim_char) {
        if (chars_to_trim[trim_char] == input_string.cstring[character]) {
          input_string.cstring[character] = '\0';
          trim_done = true;
          break;
        }
      }

      if (!trim_done) {
        break;
      }
    }
    result = true;
  }
  return result;
}

bool sl_string__trim_start(sl_string_s input_string, const char *chars_to_trim) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != chars_to_trim) &&
      sl_string__private_can_be_modified(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);
    const size_t trim_char_length = strlen(chars_to_trim);
    size_t n_chars = 0;

    for (; n_chars < input_string_length; ++n_chars) {
      bool trim_done = false;

      for (size_t trim_char = 0; trim_char < trim_char_length; ++trim_char) {
        if (chars_to_trim[trim_char] == input_string.cstring[n_chars]) {
          trim_done = true;
          break;
        }
      }

      if (!trim_done) {
        break;
      }
    }
    result = sl_string__erase_first(input_string, n_chars);
  }
  return result;
}

bool sl_string__replace_first(sl_string_s input_string, const char *replace, const char *replace_with) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != replace) && (NULL != replace_with)) {
    if (sl_string__private_can_be_modified(input_string)) {
      const int start_of_replace_index = sl_string__first_index_of(input_string, replace);
      result = true;

      if (sl_string_error != start_of_replace_index) {
        const size_t index = (size_t)(unsigned)start_of_replace_index;

        (void)sl_string__erase_after(input_string, index, strlen(replace));
        result = sl_string__insert_at(input_string, index, replace_with);
      }
    }
  }
  return result;
}

bool sl_string__replace_last(sl_string_s input_string, const char *replace, const char *replace_with) {
  bool result = false;

  if (sl_string__is_valid(input_string) && (NULL != replace) && (NULL != replace_with)) {
    if (sl_string__private_can_be_modified(input_string)) {
      const int start_of_replace_index = sl_string__last_index_of(input_string, replace);
      result = true;

      if (sl_string_error != start_of_replace_index) {
        const size_t index = (size_t)(unsigned)start_of_replace_index;

        (void)sl_string__erase_after(input_string, index, strlen(replace));
        result = sl_string__insert_at(input_string, index, replace_with);
      }
    }
  }
  return result;
}

int sl_string__replace_all(sl_string_s input_string, const char *replace, const char *replace_with) {
  int num_replaced = sl_string_error;

  if (sl_string__is_valid(input_string) && (NULL != replace) && (NULL != replace_with)) {
    if (sl_string__private_can_be_modified(input_string)) {
      int start_of_replace_index = sl_string__first_index_of(input_string, replace);
      num_replaced = 0;

      const size_t length_of_replace = strlen(replace);
      while (sl_string_error != start_of_replace_index) {
        const size_t index = (size_t)(unsigned)start_of_replace_index;

        (void)sl_string__erase_after(input_string, index, length_of_replace);
        if (!sl_string__insert_at(input_string, index, replace_with)) {
          (void)sl_string__insert_at(input_string, index, replace);
          num_replaced = -1;
          break;
        }

        start_of_replace_index = sl_string__first_index_of(input_string, replace);
        ++num_replaced;
      }
    }
  }
  return num_replaced;
}

bool sl_string__contains_only_hex_number(const sl_string_s input_string) {
  bool is_hex = false;
  if (sl_string__is_valid(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);

    if (sl_string__begins_with_ignore_case(input_string, "0x") && (input_string_length > 2U)) {
      // starting from index 2 to ignore hex notation
      const size_t start_index = 2;
      for (size_t index = start_index; index < input_string_length; ++index) {
        if (isxdigit((int)(input_string.cstring[index])) > 0) { // lint !e9029 ignoring mismatch type with binary ops
          is_hex = true;
        } else {
          is_hex = false;
          break;
        }
      }
    }
  }
  return is_hex;
}

bool sl_string__contains_only_decimal_number(const sl_string_s input_string) {
  bool is_decimal = false;
  if (sl_string__is_valid(input_string)) {
    const size_t input_string_length = sl_string__get_length(input_string);

    for (size_t index = 0; index < input_string_length; ++index) {
      if (isdigit((int)(input_string.cstring[index])) > 0) { // lint !e9029 ignoring mismatch type with binary operators
        is_decimal = true;
      } else {
        is_decimal = false;
        break;
      }
    }
  }
  return is_decimal;
}

bool sl_string__contains_only_number(const sl_string_s input_string) {
  bool is_number = false;
  if (sl_string__is_valid(input_string)) {
    is_number = sl_string__contains_only_hex_number(input_string);
    is_number = (sl_string__contains_only_decimal_number(input_string) || is_number);
  }
  return is_number;
}

char *sl_string__strnstr(const sl_string_s whole_string, const char *match_string, size_t length_to_search) {
  char *found_string = NULL;

  if (NULL != match_string) {
    const size_t match_string_length = strlen(match_string);

    if (!sl_string__is_valid(whole_string)) {
      NOOP("Invalid inputs");
    } else if (length_to_search < match_string_length) {
      NOOP("Invalid length");
    } else if (0U == match_string_length) {
      found_string = whole_string.cstring;
    } else {
      bool is_search_done = false;

      for (size_t whole_string_offset = 0U;
           (whole_string_offset <= (length_to_search - match_string_length)) && (!is_search_done);
           ++whole_string_offset) {
        char *const offset_string = (whole_string.cstring + whole_string_offset);

        if ('\0' == offset_string[0U]) {
          is_search_done = true;
        } else if (0 == strncmp(offset_string, match_string, match_string_length)) {
          found_string = offset_string;
          is_search_done = true;
        } else {
          NOOP("Nothing found; continue search");
        }
      }
    }
  }

  return found_string;
}
