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
/* Standard Includes */
#include <stdio.h>
#include <string.h>

#include "unity.h"

/* Mock Includes */

/* External Includes */

/* Module Includes */
#include "sl_string.h"

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

static const char *test_very_long_string =
    "test test test test test test test test test test test test test test test test test test test test test";
static const char *test_very_long_string_2 =
    "test test test test test test test test test test test test test test test test test";

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

static char string_memory[110];
static sl_string_s str;
static sl_string_s uninitialized_str;
static const size_t string_memory_pad_size = 10;
static const char *hello = "hello world";

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

/***********************************************************************************************************************
 *
 *                                     T E S T   S E T U P   &   T E A R D O W N
 *
 **********************************************************************************************************************/
void setUp(void) {
  memset(&uninitialized_str, 0, sizeof(uninitialized_str));
  memset(string_memory, 'p', sizeof(string_memory));

  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
}

void tearDown(void) {
  const char *expected_str = "pppppppppp";
  TEST_ASSERT_EQUAL_STRING(expected_str, string_memory + sizeof(string_memory) - string_memory_pad_size);
}

/***********************************************************************************************************************
 *
 *                                                     T E S T S
 *
 **********************************************************************************************************************/

void test__initialize_successful(void) { TEST_ASSERT_EQUAL_PTR(string_memory, str.cstring); }

void test__initialize_unsuccessful_null_mem(void) {
  const sl_string_s null_str = sl_string__initialize(NULL, sizeof(string_memory) - string_memory_pad_size);
  TEST_ASSERT_NULL(null_str.cstring);
}

void test__initialize_unsuccessful_mem_size_zero(void) {
  const sl_string_s null_str = sl_string__initialize(string_memory, 0);
  TEST_ASSERT_NULL(null_str.cstring);
}

void test__initialize_from_successful(void) {
  str = sl_string__initialize_from(string_memory, sizeof(string_memory) - string_memory_pad_size, "Hello World");

  TEST_ASSERT_EQUAL_PTR(string_memory, str.cstring);
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__initialize_from_null_init_string(void) {
  str = sl_string__initialize_from(string_memory, sizeof(string_memory) - string_memory_pad_size, NULL);
  TEST_ASSERT_NULL(str.cstring);
}

void test__initialize_from_zero_memory_size_max(void) {
  TEST_ASSERT_NULL(sl_string__initialize_from(string_memory, 0, "Hello World").cstring);
}

void test__initialize_from_null_string_memory(void) {
  str = sl_string__initialize_from(NULL, 0, "test");
  TEST_ASSERT_NULL(str.cstring);
}

void test__sl_string__initialize_non_modifiable_nullptr(void) {
  str = sl_string__initialize_non_modifiable(NULL);
  TEST_ASSERT_NULL(str.cstring);
  TEST_ASSERT_EQUAL(0, sl_string__get_capacity(str));
}

void test__sl_string__initialize_non_modifiable(void) {
  str = sl_string__initialize_non_modifiable(hello);
  TEST_ASSERT_EQUAL(strlen(hello), sl_string__get_length(str));
  TEST_ASSERT_EQUAL(0, sl_string__get_capacity(str));
  TEST_ASSERT_EQUAL(0, sl_string__get_capacity_including_null_char(str));
}

void test__apis_that_do_not_work_on_non_modifiable(void) {
  const char *word = "word";
  str = sl_string__initialize_non_modifiable(hello);

  TEST_ASSERT_FALSE(sl_string__clear(str));
  TEST_ASSERT_FALSE(sl_string__clear_all_memory(str));
  TEST_ASSERT_FALSE(sl_string__to_lower(str));
  TEST_ASSERT_FALSE(sl_string__to_upper(str));

  char *token = NULL;
  TEST_ASSERT_FALSE(-1 != sl_string__tokenize(str, ",", 1, &token));

  va_list test_args;
  TEST_ASSERT_FALSE(0 != sl_string__printf(str, "123"));
  TEST_ASSERT_FALSE(0 != sl_string__vprintf(str, "123", test_args));
  TEST_ASSERT_FALSE(0 != sl_string__printf_append(str, "123"));
  TEST_ASSERT_FALSE(0 != sl_string__vprintf_append(str, "123", test_args));

  TEST_ASSERT_FALSE(sl_string__set(str, word));
  TEST_ASSERT_FALSE(sl_string__insert_at(str, 0, word));
  TEST_ASSERT_FALSE(sl_string__append(str, word));
  TEST_ASSERT_FALSE(sl_string__append_char(str, 'c'));
  TEST_ASSERT_FALSE(sl_string__append_truncate(str, word));

  TEST_ASSERT_FALSE(sl_string__erase(str, word));
  TEST_ASSERT_FALSE(sl_string__erase_first(str, 1));
  TEST_ASSERT_FALSE(sl_string__erase_last(str, 1));
  TEST_ASSERT_FALSE(sl_string__erase_at(str, 1));
  TEST_ASSERT_FALSE(sl_string__erase_after(str, 1, 1));
  TEST_ASSERT_FALSE(sl_string__erase_at_substring(str, word));
  TEST_ASSERT_FALSE(sl_string__erase_first_word(str, ' '));
  TEST_ASSERT_FALSE(0 != sl_string__erase_special_chars(str));

  long some_int;
  TEST_ASSERT_FALSE(sl_string__erase_int(str, &some_int));
  TEST_ASSERT_FALSE(sl_string__trim_end(str, word));
  TEST_ASSERT_FALSE(sl_string__trim_start(str, word));
  TEST_ASSERT_FALSE(sl_string__replace_first(str, word, word));
  TEST_ASSERT_FALSE(sl_string__replace_last(str, word, word));
  TEST_ASSERT_FALSE(-1 != sl_string__replace_all(str, word, word));

  // Should remain non modified from sl_string__initialize_non_modifiable()
  TEST_ASSERT_TRUE(sl_string__equals_to(str, hello));
}

void test__get_capacity_fails_with_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_UINT(0U, sl_string__get_capacity(uninitialized_str));
}

void test__get_capacity(void) {
  TEST_ASSERT_EQUAL_UINT(sizeof(string_memory) - string_memory_pad_size - 1, sl_string__get_capacity(str));
}

void test__get_capacity_including_null_char_fails_with_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_UINT(0U, sl_string__get_capacity_including_null_char(uninitialized_str));
}

void test__is_full_returns_true_with_uninitialized_string(void) {
  TEST_ASSERT_TRUE(sl_string__is_full(uninitialized_str));
}

void test__is_full(void) {
  char mem[3];
  str = sl_string__initialize(mem, sizeof(mem));

  TEST_ASSERT_FALSE(sl_string__is_full(str));
  sl_string__append(str, "a");
  TEST_ASSERT_FALSE(sl_string__is_full(str));
  sl_string__append(str, "b");
  TEST_ASSERT_TRUE(sl_string__is_full(str));
}

void test__get_length_empty_string(void) { TEST_ASSERT_EQUAL_UINT(0, sl_string__get_length(str)); }

void test__get_length_null_string(void) { TEST_ASSERT_EQUAL_UINT(0, sl_string__get_length(uninitialized_str)); }

void test__set_fails_with_null_string_pointer(void) { TEST_ASSERT_FALSE(sl_string__set(uninitialized_str, hello)); }

void test__set_fails_with_null_source_pointer(void) { TEST_ASSERT_FALSE(sl_string__set(str, NULL)); }

void test__get_length_non_empty_string(void) {
  sl_string__set(str, hello);
  TEST_ASSERT_EQUAL_UINT(strlen(hello), sl_string__get_length(str));
}

void test__c_str_null_string(void) { TEST_ASSERT_EQUAL(NULL, sl_string__c_str(uninitialized_str)); }

void test__c_str_empty_string(void) { TEST_ASSERT_EQUAL_STRING("", sl_string__c_str(str)); }

void test__c_str_non_empty_string(void) {
  sl_string__set(str, hello);
  TEST_ASSERT_EQUAL_STRING(hello, sl_string__c_str(str));
}

void test__get_modifiable_c_str_null_string(void) {
  TEST_ASSERT_EQUAL(NULL, sl_string__get_modifiable_c_str(uninitialized_str));
}

void test__get_modifiable_c_str_empty_string(void) {
  TEST_ASSERT_EQUAL_STRING("", sl_string__get_modifiable_c_str(str));
}

void test__get_modifiable_c_str_non_empty_string(void) {
  sl_string__set(str, hello);
  TEST_ASSERT_EQUAL_STRING(hello, sl_string__get_modifiable_c_str(str));
}

void test__get_modifiable_c_str_on_non_mondifiable(void) {
  str = sl_string__initialize_non_modifiable("non modifiable");
  TEST_ASSERT_NULL(sl_string__get_modifiable_c_str(str));
}

void test__append_char_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__append_char(uninitialized_str, 'a'));
}

void test__append_char(void) {
  char memory[3 + 1];
  sl_string_s s = sl_string__initialize(memory, sizeof(memory));
  TEST_ASSERT_EQUAL_STRING(s.cstring, "");
  TEST_ASSERT_EQUAL(3, sl_string__get_capacity(s));
  TEST_ASSERT_EQUAL(0, sl_string__get_length(s));

  sl_string__append_char(s, 'a');
  TEST_ASSERT_EQUAL_STRING(s.cstring, "a");
  TEST_ASSERT_EQUAL(1, sl_string__get_length(s));

  sl_string__append_char(s, 'b');
  TEST_ASSERT_EQUAL_STRING(s.cstring, "ab");
  TEST_ASSERT_EQUAL(2, sl_string__get_length(s));

  sl_string__append_char(s, 'c');
  TEST_ASSERT_EQUAL_STRING(s.cstring, "abc");
  TEST_ASSERT_EQUAL(3, sl_string__get_length(s));

  sl_string__append_char(s, 'd');
  TEST_ASSERT_EQUAL_STRING(s.cstring, "abc");
  TEST_ASSERT_EQUAL(3, sl_string__get_length(s));
}

void test__to_int_fails_with_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_INT(0, sl_string__to_int(uninitialized_str));
}

void test__to_int_successful(void) {
  sl_string__set(str, "123");
  TEST_ASSERT_EQUAL_INT(123, sl_string__to_int(str));

  sl_string__set(str, "-123");
  TEST_ASSERT_EQUAL_INT(-123, sl_string__to_int(str));
}

void test__from_hexint(void) {
  sl_string__set(str, "0x123");
  TEST_ASSERT_EQUAL_INT(0x123, sl_string__to_int(str));

  sl_string__set(str, "x123");
  TEST_ASSERT_EQUAL_INT(0, sl_string__to_int(str));
}

void test__to_int_unsuccessful_contains_non_int_chars(void) {
  sl_string__set(str, "Hello 123 Word");
  TEST_ASSERT_EQUAL_INT(0, sl_string__to_int(str));

  sl_string__set(str, "$123#");
  TEST_ASSERT_EQUAL_INT(0, sl_string__to_int(str));

  sl_string__set(str, "Hello $123# Word");
  TEST_ASSERT_EQUAL_INT(0, sl_string__to_int(str));
}

void test__to_int_unsuccessful_empty_string(void) { TEST_ASSERT_EQUAL_INT(0, sl_string__to_int(str)); }

void test__to_float_fails_with_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_FLOAT(0.0, sl_string__to_float(uninitialized_str));
}

void test__to_float_successful(void) {
  sl_string__set(str, "1.23");
  TEST_ASSERT_EQUAL_FLOAT(1.23, sl_string__to_float(str));

  sl_string__set(str, "-1.23");
  TEST_ASSERT_EQUAL_FLOAT(-1.23, sl_string__to_float(str));

  sl_string__set(str, "+1.23f");
  TEST_ASSERT_EQUAL_FLOAT(1.23, sl_string__to_float(str));
}

void test__to_float_unsuccessful_contains_non_float_chars(void) {
  sl_string__set(str, "Hello 1.23 Word");
  TEST_ASSERT_EQUAL_FLOAT(0.0, sl_string__to_float(str));

  sl_string__set(str, "$1.23#");
  TEST_ASSERT_EQUAL_FLOAT(0.0, sl_string__to_float(str));

  sl_string__set(str, "Hello $1.23# Word");
  TEST_ASSERT_EQUAL_FLOAT(0.0, sl_string__to_float(str));
}

void test__to_float_unsuccessful_empty_string(void) { TEST_ASSERT_EQUAL_FLOAT(0.0, sl_string__to_float(str)); }

void test__is_alpha_contains_only_alpha_chars(void) {
  sl_string__set(str, "HelloWorld");
  TEST_ASSERT_TRUE(sl_string__is_alpha(str));
}

void test__is_alpha_contains_non_alpha_chars(void) {
  sl_string__set(str, "Hello$123# Word");
  TEST_ASSERT_FALSE(sl_string__is_alpha(str));
}

void test__is_alpha_fails_with_null_string_pointer(void) { TEST_ASSERT_FALSE(sl_string__is_alpha(uninitialized_str)); }

void test__is_alpha_empty_string(void) { TEST_ASSERT_TRUE(sl_string__is_alpha(str)); }

void test__is_alphanum_successful(void) {
  sl_string__set(str, "Hello123Word");
  TEST_ASSERT_TRUE(sl_string__is_alphanum(str));

  sl_string__set(str, "HelloWorld");
  TEST_ASSERT_TRUE(sl_string__is_alphanum(str));

  sl_string__set(str, "123");
  TEST_ASSERT_TRUE(sl_string__is_alphanum(str));
}

void test__is_alphanum_contains_non_alphanum_chars(void) {
  sl_string__set(str, "Hello $123#Word");
  TEST_ASSERT_FALSE(sl_string__is_alphanum(str));
}

void test__is_alphanum_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__is_alphanum(uninitialized_str));
}

void test__is_alphanum_empty_string(void) { TEST_ASSERT_TRUE(sl_string__is_alphanum(str)); }

void test__clear_fails_with_null_string_pointer(void) { TEST_ASSERT_FALSE(sl_string__clear(uninitialized_str)); }

void test__clear(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__clear(str));
  TEST_ASSERT_EQUAL_STRING("", sl_string__c_str(str));
}

void test__clear_all_memory_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__clear_all_memory(uninitialized_str));
}

void test__clear_all_memory(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__clear_all_memory(str));
  for (uint16_t character = 0; character < sizeof(string_memory) - string_memory_pad_size; character++) {
    TEST_ASSERT_EQUAL_UINT8('\0', str.cstring[0]);
  }

  TEST_ASSERT_EQUAL_STRING("", sl_string__c_str(str));
}

void test__to_lower_fails_with_null_string_pointer(void) { TEST_ASSERT_FALSE(sl_string__to_lower(uninitialized_str)); }

void test__to_lower(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__to_lower(str));
  TEST_ASSERT_EQUAL_STRING("hello world", sl_string__c_str(str));
}

void test__to_upper_fails_with_null_string_pointer(void) { TEST_ASSERT_FALSE(sl_string__to_upper(uninitialized_str)); }

void test__to_upper(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__to_upper(str));
  TEST_ASSERT_EQUAL_STRING("HELLO WORLD", sl_string__c_str(str));
}

void test__printf_fails_with_null_string_pointer(void) {
  const int printed_count =
      sl_string__printf(uninitialized_str, "String1 %s, Int: %d, String2: %s", "Hello", 123, "World");
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__printf_fails_with_null_format_pointer(void) {
  const int printed_count = sl_string__printf(str, NULL, "Hello", 123, "World");
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__printf_successful(void) {
  const int printed_count = sl_string__printf(str, "String1 %s, Int: %d, String2: %s", "Hello", 123, "World");
  const char *expected_str = "String1 Hello, Int: 123, String2: World";

  TEST_ASSERT_EQUAL_INT(strlen(expected_str), printed_count);
  TEST_ASSERT_EQUAL_STRING(expected_str, sl_string__c_str(str));
}

void test__vprintf_fails_with_null_string_pointer(void) {
  va_list test_args;
  TEST_ASSERT_EQUAL(0U, sl_string__vprintf(uninitialized_str, "String1 %s, Int: %d, String2: %s", test_args));
  TEST_ASSERT_EQUAL(0U, sl_string__vprintf(str, NULL, test_args));
}

void test__vprintf_fails_with_null_format_pointer(void) {
  va_list test_args;
  const size_t printed_count = sl_string__vprintf(uninitialized_str, "Test String", test_args);
  TEST_ASSERT_EQUAL(0U, printed_count);
}

void test__copy_to_fails_with_null_string_pointer(void) {
  char copy_buffer[8];
  TEST_ASSERT_EQUAL(0, sl_string__copy_to(uninitialized_str, copy_buffer, 0));
}

void test__copy_to(void) {
  char copy_buffer[8];

  TEST_ASSERT_EQUAL(0, sl_string__copy_to(str, copy_buffer, 0));

  sl_string__set(str, "hello");
  TEST_ASSERT_EQUAL(0, sl_string__copy_to(str, copy_buffer, 0));

  TEST_ASSERT_EQUAL(0, sl_string__copy_to(str, copy_buffer, 1));
  TEST_ASSERT_EQUAL_STRING("", copy_buffer);

  TEST_ASSERT_EQUAL(1, sl_string__copy_to(str, copy_buffer, 2));
  TEST_ASSERT_EQUAL_STRING("h", copy_buffer);

  TEST_ASSERT_EQUAL(4, sl_string__copy_to(str, copy_buffer, 5));
  TEST_ASSERT_EQUAL_STRING("hell", copy_buffer);

  TEST_ASSERT_EQUAL(5, sl_string__copy_to(str, copy_buffer, 6));
  TEST_ASSERT_EQUAL_STRING("hello", copy_buffer);

  TEST_ASSERT_EQUAL(5, sl_string__copy_to(str, copy_buffer, 7));
  TEST_ASSERT_EQUAL_STRING("hello", copy_buffer);

  TEST_ASSERT_EQUAL(0, sl_string__copy_to(str, NULL, 0));
  TEST_ASSERT_EQUAL(0, sl_string__copy_to(str, NULL, 1));
  TEST_ASSERT_EQUAL(1, sl_string__copy_to(str, NULL, 2));
  TEST_ASSERT_EQUAL(5, sl_string__copy_to(str, NULL, 7));
}

void test__printf_with_string_size_zero(void) {
  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  const int printed_count = sl_string__printf(str, "%s", "");
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__printf_append_fails_with_null_string_pointer(void) {
  const int printed_count = sl_string__printf_append(uninitialized_str, "%s", "");
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__printf_append_fails_with_null_format_pointer(void) {
  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  const int printed_count = sl_string__printf_append(str, NULL, "");
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__printf_append_with_string_size_zero(void) {
  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  const int printed_count = sl_string__printf_append(str, "%s", "");
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__vprintf_append_fails_with_null_string_pointer(void) {
  va_list args;
  const int printed_count = sl_string__vprintf_append(uninitialized_str, "%s", args);
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__vprintf_append_fails_with_null_format_pointer(void) {
  va_list args;
  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  const int printed_count = sl_string__vprintf_append(str, NULL, args);
  TEST_ASSERT_EQUAL_INT(0, printed_count);
}

void test__printf_filename_format(void) {
  const char *expected_filename = "logs/debuglog__predump.in_use";
  const char *directory_name = "logs";
  const char *preceding_name = "debuglog";
  const char *tagname = "predump";
  sl_string_s file_name = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  const int printed_count = sl_string__printf(file_name, "%s/%s__%s.in_use", directory_name, preceding_name, tagname);
  TEST_ASSERT_EQUAL_INT(strlen(file_name.cstring), printed_count);
  TEST_ASSERT_EQUAL_STRING(expected_filename, file_name.cstring);
}

void test__scanf_fails_with_null_string_pointer(void) {
  char *hello_str = "";
  int parsed_count = sl_string__scanf(uninitialized_str, "%s", hello_str);
  TEST_ASSERT_EQUAL_INT(0, parsed_count);
}

void test__scanf_fails_with_null_format_pointer(void) {
  char *hello_str = "";
  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  int parsed_count = sl_string__scanf(str, NULL, hello_str);
  TEST_ASSERT_EQUAL_INT(0, parsed_count);
}

void test__scanf_with_arg_size_zero(void) {
  char *hello_str = "";
  str = sl_string__initialize(string_memory, sizeof(string_memory) - string_memory_pad_size);
  int parsed_count = sl_string__scanf(str, "%s", hello_str);
  TEST_ASSERT_EQUAL_INT(0, parsed_count);
}

void test__printf_truncated(void) {
  const int printed_count =
      sl_string__printf(str, "String1 %s, Int: %d, String2: %s", test_very_long_string, 123, "test");
  const char *expected_str =
      "String1 test test test test test test test test test test test test test test test test test test t";
  const char *expected_str_before_truncation =
      "String1 test test test test test test test test test test test test test test test test test test test test "
      "test, Int: 123, String2: test";

  TEST_ASSERT_EQUAL_INT(strlen(expected_str_before_truncation), printed_count);
  TEST_ASSERT_EQUAL_STRING(expected_str, str.cstring);
}

void test__printf_append_successful(void) {
  sl_string__append(str, "Append ");
  const int printed_count = sl_string__printf_append(str, "String1 %s, Int: %d, String2: %s", "Hello", 123, "World");
  const char *expected_str_appended = "String1 Hello, Int: 123, String2: World";
  const char *expected_str = "Append String1 Hello, Int: 123, String2: World";

  TEST_ASSERT_EQUAL_INT(strlen(expected_str_appended), printed_count);
  TEST_ASSERT_EQUAL_STRING(expected_str, str.cstring);
}

void test__printf_append_truncated(void) {
  sl_string__append(str, "Append ");
  int printed_count =
      sl_string__printf_append(str, "String1 %s, Int: %d, String2: %s", test_very_long_string, 123, "test");
  const char *expected_str =
      "Append String1 test test test test test test test test test test test test test test test test test";
  const char *expected_str_appended_before_truncation =
      "String1 test test test test test test test test test test test test test test test test test test test test "
      "test, Int: 123, String2: test";

  TEST_ASSERT_EQUAL_INT(strlen(expected_str_appended_before_truncation), printed_count);
  TEST_ASSERT_EQUAL_STRING(expected_str, str.cstring);

  printed_count = sl_string__printf_append(str, "%s", test_very_long_string);
  TEST_ASSERT_EQUAL_INT(strlen(test_very_long_string), printed_count);
  TEST_ASSERT_EQUAL_STRING(expected_str, str.cstring);
}

void test__scanf_successful(void) {
  char string1[20];
  char string2[20];
  int num = 0;

  sl_string__set(str, "Hello 123 World");
  const int scanned_count = sl_string__scanf(str, "%19s %d %19s", string1, &num, string2);

  TEST_ASSERT_EQUAL_INT(3, scanned_count);
  TEST_ASSERT_EQUAL_STRING("Hello", string1);
  TEST_ASSERT_EQUAL_INT(123, num);
  TEST_ASSERT_EQUAL_STRING("World", string2);
}

void test__tokenize_fails_with_null_string_pointer(void) {
  char *one = NULL;
  char *two = NULL;
  char *three = NULL;
  TEST_ASSERT_EQUAL_INT(-1, sl_string__tokenize(uninitialized_str, " ", 3, &one, &two, &three));
}
//
void test__tokenize_fails_with_null_delimiter_string_pointer(void) {
  sl_string__set(str, "Hello 123 World");
  char *one = NULL;
  char *two = NULL;
  char *three = NULL;
  TEST_ASSERT_EQUAL_INT(-1, sl_string__tokenize(str, NULL, 3, &one, &two, &three));
}

void test__tokenize_successful(void) {
  sl_string__set(str, "Hello 123 World");

  char *one = NULL;
  char *two = NULL;
  char *three = NULL;

  TEST_ASSERT_EQUAL_INT(3, sl_string__tokenize(str, " ", 3, &one, &two, &three));

  TEST_ASSERT_EQUAL_STRING("Hello", one);
  TEST_ASSERT_EQUAL_STRING("123", two);
  TEST_ASSERT_EQUAL_STRING("World", three);
}

void test__tokenize_string_is_token(void) {
  sl_string__set(str, "Hello");

  char *one = NULL;
  char *two = NULL;

  TEST_ASSERT_EQUAL_INT(0, sl_string__tokenize(str, "Hello", 2, &one, &two));

  TEST_ASSERT_EQUAL_PTR(NULL, one);
  TEST_ASSERT_EQUAL_PTR(NULL, two);
}

void test__tokenize_unsuccessful_null_delimiter(void) {
  sl_string__set(str, "Hello 123 Word");

  char *one = NULL;
  char *two = NULL;
  char *three = NULL;

  TEST_ASSERT_EQUAL_INT(-1, sl_string__tokenize(str, NULL, 3, &one, &two, &three));

  TEST_ASSERT_EQUAL_STRING(NULL, one);
  TEST_ASSERT_EQUAL_STRING(NULL, two);
  TEST_ASSERT_EQUAL_STRING(NULL, three);
}

void test__tokenize_null_args(void) {
  sl_string__set(str, "Hello,123,Word");
  TEST_ASSERT_EQUAL_INT(1, sl_string__tokenize(str, ",", 1, NULL));
}

void test__tokenize_mismatch_of_ptrs_and_num_of_tokens(void) {
  sl_string__set(str, "Hello 123 Word");

  char *one = NULL;
  char *two = NULL;
  char *three = NULL;

  TEST_ASSERT_EQUAL_INT(2, sl_string__tokenize(str, " ", 2, &one, &two));

  TEST_ASSERT_EQUAL_STRING("Hello", one);
  TEST_ASSERT_EQUAL_STRING("123", two);
  TEST_ASSERT_EQUAL_STRING(NULL, three);
}

void test__set(void) {
  TEST_ASSERT_TRUE(sl_string__set(str, "hello"));
  TEST_ASSERT_EQUAL_STRING("hello", str.cstring);

  // When string does not have enough memory (capacity is 2 when memory length is 5)
  str = sl_string__initialize(string_memory, 5);
  TEST_ASSERT_TRUE(sl_string__set(str, "hi"));
  TEST_ASSERT_EQUAL_STRING("hi", str.cstring);
  TEST_ASSERT_TRUE(sl_string__set(str, "I"));
  TEST_ASSERT_EQUAL_STRING("I", str.cstring);

  TEST_ASSERT_FALSE(sl_string__set(str, "aaaaaaaaaaaaaaaaa"));
  TEST_ASSERT_EQUAL_STRING("I", str.cstring);
}

void test__insert_at_fails_with_null_string_pointer(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__insert_at(uninitialized_str, 5, "Hello 123 World"));
}

void test__insert_at_success(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__insert_at(str, 5, "Hello 123 World"));
  TEST_ASSERT_EQUAL_STRING("HelloHello 123 World World", str.cstring);
}

void test__insert_at_pos_out_of_bounds(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__insert_at(str, sizeof("Hello World"), "Hello 123 Word"));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__insert_at_string_is_null(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__insert_at(str, 5, NULL));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__insert_at_capacity_reached(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__insert_at(str, 5, test_very_long_string));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__append_successful(void) {
  TEST_ASSERT_TRUE(sl_string__append(str, "Hello World"));

  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__append_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__append(uninitialized_str, test_very_long_string));
}

void test__append_string_is_null(void) {
  TEST_ASSERT_FALSE(sl_string__append(str, NULL));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__append_capacity_reached(void) {
  TEST_ASSERT_FALSE(sl_string__append(str, test_very_long_string));

  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__append_truncate_fails_with_null_string_pointer(void) {
  const char *hello_world = "Hello World";
  TEST_ASSERT_EQUAL(0U, sl_string__append_truncate(uninitialized_str, hello_world));
}

void test__append_truncate_no_truncate_needed(void) {
  const char *hello_world = "Hello World";
  TEST_ASSERT_EQUAL(strlen(hello_world), sl_string__append_truncate(str, hello_world));
  TEST_ASSERT_EQUAL_STRING(hello_world, str.cstring);
}

void test__append_truncate_string_is_null(void) {
  TEST_ASSERT_EQUAL(0U, sl_string__append_truncate(str, NULL));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__append_truncate_truncate_string(void) {
  const char *test_very_long_string_truncated =
      "test test test test test test test test test test test test test test test test test test test test";
  const char *test_very_long_string_truncated_2 =
      "test test test test test test test test test test test test test test test test test test test tesA";
  TEST_ASSERT_EQUAL(sl_string__get_capacity(str), sl_string__append_truncate(str, test_very_long_string));
  TEST_ASSERT_EQUAL(sl_string__get_capacity(str), sl_string__get_length(str));
  TEST_ASSERT_EQUAL_STRING(test_very_long_string_truncated, str.cstring);

  TEST_ASSERT_EQUAL(0U, sl_string__append_truncate(str, "Should not append"));
  TEST_ASSERT_EQUAL_STRING(test_very_long_string_truncated, str.cstring);
  TEST_ASSERT_EQUAL(0U, sl_string__append_truncate(str, ""));
  TEST_ASSERT_EQUAL_STRING(test_very_long_string_truncated, str.cstring);
  TEST_ASSERT_TRUE(sl_string__is_full(str));
  TEST_ASSERT_TRUE(sl_string__erase_last(str, 1U));
  TEST_ASSERT_FALSE(sl_string__is_full(str));
  TEST_ASSERT_EQUAL(strlen("A"), sl_string__append_truncate(str, "A"));
  TEST_ASSERT_TRUE(sl_string__is_full(str));
  TEST_ASSERT_EQUAL(0U, sl_string__append_truncate(str, "Hello"));
  TEST_ASSERT_EQUAL_STRING(test_very_long_string_truncated_2, str.cstring);
}

void test__append_truncate_untruncated_multple_append(void) {
  TEST_ASSERT_EQUAL(strlen("Hello "), sl_string__append_truncate(str, "Hello "));
  TEST_ASSERT_EQUAL(strlen("World!"), sl_string__append_truncate(str, "World!"));
  TEST_ASSERT_EQUAL_STRING("Hello World!", sl_string__c_str(str));
  TEST_ASSERT_EQUAL(strlen(""), sl_string__append_truncate(str, ""));
  TEST_ASSERT_EQUAL_STRING("Hello World!", sl_string__c_str(str));
}

void test__append_truncate_empty_string(void) {
  TEST_ASSERT_EQUAL(strlen(""), sl_string__append_truncate(str, ""));
  TEST_ASSERT_EQUAL_STRING("", sl_string__c_str(str));
  TEST_ASSERT_EQUAL(strlen("World!"), sl_string__append_truncate(str, "World!"));
  TEST_ASSERT_EQUAL_STRING("World!", sl_string__c_str(str));
}

void test__append_truncate_add_nothing_many_times(void) {
  for (size_t char_iterator = 0; char_iterator <= sl_string__get_capacity(str); ++char_iterator) {
    TEST_ASSERT_EQUAL(strlen(""), sl_string__append_truncate(str, ""));
  }
  TEST_ASSERT_EQUAL(strlen("Hello World!"), sl_string__append_truncate(str, "Hello World!"));
  TEST_ASSERT_EQUAL_STRING("Hello World!", sl_string__c_str(str));
}

void test__append_truncate_string_is_exactly_capacity(void) {
  const char *append_truncate = "abcdefghikjlmn";
  TEST_ASSERT_EQUAL(strlen(test_very_long_string_2), sl_string__append_truncate(str, test_very_long_string_2));
  TEST_ASSERT_EQUAL(strlen(append_truncate) + 1U, sl_string__get_capacity(str) - sl_string__get_length(str));
  TEST_ASSERT_EQUAL(strlen(append_truncate), sl_string__append_truncate(str, append_truncate));
}

void test__equals_to_successful(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__equals_to(str, "Hello World"));
}

void test__equals_to_unsuccessful_strings_dont_match(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__equals_to(str, "hello world"));
}

void test__equals_to_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__equals_to(str, NULL));
}

void test__equals_to_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__equals_to(uninitialized_str, "Hello World"));
}

void test__equals_to_ignore_case_unsuccessful_null_string_pointer(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__equals_to_ignore_case(uninitialized_str, "Hello 123 Word"));
}

void test__equals_to_ignore_case_successful(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__equals_to_ignore_case(str, "HelLo WoRld"));
  TEST_ASSERT_TRUE(sl_string__equals_to_ignore_case(str, "hello world"));
}

void test__equals_to_ignore_case_unsuccessful_strings_dont_match(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__equals_to_ignore_case(str, "Hello 123 Word"));
}

void test__equals_to_ignore_case_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__equals_to_ignore_case(str, NULL));
}

void test__last_index_of_found(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(12, sl_string__last_index_of(str, "Hello"));
}

void test__last_index_of_unsuccessful_not_found(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__last_index_of(str, "hello"));
}

void test__last_index_of_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__last_index_of(str, NULL));
}

void test__last_index_of_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_INT(-1, sl_string__last_index_of(uninitialized_str, "hello"));
}

void test__last_index_of_ignore_case_found(void) {
  sl_string__printf(str, "Hello World Hello World");

  TEST_ASSERT_EQUAL_INT(12, sl_string__last_index_of_ignore_case(str, "hello"));

  sl_string__printf(str, "Hello World HHello World");

  TEST_ASSERT_EQUAL_INT(13, sl_string__last_index_of_ignore_case(str, "hello"));
}

void test__last_index_of_ignore_lower_case(void) {
  sl_string__set(str, "hello WORLD");
  TEST_ASSERT_EQUAL_INT(0, sl_string__last_index_of_ignore_case(str, "HELLO"));
}

void test__last_index_of_ignore_multiple_sub_string(void) {
  sl_string__set(str, "hello WORLD hello hello");
  TEST_ASSERT_EQUAL_INT(18, sl_string__last_index_of_ignore_case(str, "hello"));
}

void test__last_index_of_ignore_case_not_found(void) {
  TEST_ASSERT_EQUAL_INT(-1, sl_string__last_index_of_ignore_case(str, "hey"));
}

void test__last_index_of_ignore_case_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__last_index_of_ignore_case(str, NULL));
}

void test__last_index_of_ignore_case_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_INT(-1, sl_string__last_index_of_ignore_case(uninitialized_str, "hello"));
}

void test__first_index_of_found(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(0, sl_string__first_index_of(str, "Hello"));

  sl_string__set(str, "HHello World Hello World");
  TEST_ASSERT_EQUAL_INT(1, sl_string__first_index_of(str, "Hello"));
}

void test__first_index_of_unsuccessful_not_found(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__first_index_of(str, "hello"));
}

void test__first_index_of_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__first_index_of(str, NULL));
}

void test__first_index_of_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_INT(-1, sl_string__first_index_of(uninitialized_str, "hello"));
}

void test__first_index_of_ignore_case_found(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(0, sl_string__first_index_of_ignore_case(str, "hello"));

  sl_string__set(str, "HHello World Hello World");
  TEST_ASSERT_EQUAL_INT(1, sl_string__first_index_of_ignore_case(str, "Hello"));

  sl_string__set(str, "hefllo World hHello World");
  TEST_ASSERT_EQUAL_INT(14, sl_string__first_index_of_ignore_case(str, "Hello"));
}

void test__first_index_of_ignore_case_not_found(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__first_index_of_ignore_case(str, "hey"));
}

void test__first_index_of_ignore_case_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__first_index_of_ignore_case(str, NULL));
}

void test__first_index_of_ignore_case_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_INT(-1, sl_string__first_index_of_ignore_case(uninitialized_str, "hello"));
}

void test__first_index_of_ignore_lower_case(void) {
  sl_string__set(str, "hello HeLLO WORLD");
  TEST_ASSERT_EQUAL_INT(0, sl_string__first_index_of_ignore_case(str, "HeLLO"));
}

void test__contains_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__contains(str, "Hello"));
}

void test__contains_unsuccessful_not_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__contains(str, "hello"));
}

void test__contains_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__contains(str, NULL));
}

void test__contains_unsuccessful_null_string_pointer(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__contains(uninitialized_str, "hello"));
}

void test__contains_ignore_case_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__contains_ignore_case(str, "hello"));
}

void test__contains_ignore_case_not_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__contains_ignore_case(str, "hey"));
}

void test__contains_ignore_case_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__contains_ignore_case(str, NULL));
}

void test__contains_ignore_case_unsuccessful_null_string_pointer(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__contains_ignore_case(uninitialized_str, "hey"));
}

void test__count_of_successful(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(2, sl_string__count_of(str, "World"));
}

void test__count_of_no_occurance(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(0, sl_string__count_of(str, "hey"));
}

void test__count_of_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__count_of(str, NULL));
}

void test__count_of_unsuccessful_null_input_string(void) {
  sl_string__set(str, "Hello World Hello World");
  TEST_ASSERT_EQUAL_INT(-1, sl_string__count_of(uninitialized_str, "hey"));
}

void test__begins_with_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__begins_with(str, "Hello"));
}

void test__begins_with_unsuccessful_not_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__begins_with(str, "World"));
}

void test__begins_with_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__begins_with(str, NULL));
}

void test__begins_with_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__begins_with(uninitialized_str, "hello"));
}

void test__begins_unsuccessful_large_substring(void) {
  sl_string__set(str, "world");
  TEST_ASSERT_FALSE(sl_string__begins_with(str, "Hello World"));
}

void test__begins_with_limited_chars(void) {
  sl_string__set(str, "test begins with");

  TEST_ASSERT_TRUE(sl_string__begins_with_limited_chars(str, "test", 1));
  TEST_ASSERT_TRUE(sl_string__begins_with_limited_chars(str, "test", 2));
  TEST_ASSERT_TRUE(sl_string__begins_with_limited_chars(str, "test", 3));
  TEST_ASSERT_TRUE(sl_string__begins_with_limited_chars(str, "test", 4));

  TEST_ASSERT_TRUE(sl_string__begins_with_limited_chars(str, "test ", 5));
  TEST_ASSERT_TRUE(sl_string__begins_with_limited_chars(str, "test b", 6));

  TEST_ASSERT_FALSE(sl_string__begins_with_limited_chars(str, "tesT", 4));
  TEST_ASSERT_FALSE(sl_string__begins_with_limited_chars(str, "ab", 2));
  TEST_ASSERT_FALSE(sl_string__begins_with_limited_chars(str, "tx", 2));
}

void test__begins_with_limited_chars_null_parameters_return_false(void) {
  sl_string__set(str, "test begins with");
  TEST_ASSERT_FALSE(sl_string__begins_with_limited_chars(uninitialized_str, "test", 1));
  TEST_ASSERT_FALSE(sl_string__begins_with_limited_chars(str, NULL, 1));
}

void test__begins_with_ignore_case_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__begins_with_ignore_case(str, "hElLo"));
}

void test__begins_with_ignore_case_not_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__begins_with_ignore_case(str, "hey"));
}

void test__begins_with_ignore_case_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__begins_with_ignore_case(str, NULL));
}

void test__begins_with_ignore_case_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__begins_with_ignore_case(uninitialized_str, "hey"));
}

void test__begins_ignore_case_unsuccessful_large_substring(void) {
  sl_string__set(str, "world");
  TEST_ASSERT_FALSE(sl_string__begins_with_ignore_case(str, "hEllo World"));
}

void test__begins_with_whole_word_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__begins_with_whole_word(str, "Hello", " "));

  sl_string__set(str, "Hello;World");
  TEST_ASSERT_TRUE(sl_string__begins_with_whole_word(str, "Hello", ";"));
}

void test__begins_with_whole_word_unsuccessful_large_substring(void) {
  sl_string__set(str, "World");
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word(str, "Hello World", ""));
}

void test__begins_with_whole_word_not_matching(void) {
  sl_string__set(str, "World");
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word(str, "orld", ""));
}

void test__begins_with_whole_word_string_ends_with_null(void) {
  sl_string__set(str, "World\0");
  TEST_ASSERT_TRUE(sl_string__begins_with_whole_word(str, "World", ","));
}

void test__begins_with_whole_word_unsuccessful_not_found(void) {
  sl_string__set(str, "HelloWorld");
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word(str, "Hello", " "));
}

void test__begins_with_whole_word_unsuccessful_null_params(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word(str, NULL, " "));
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word(str, "Hello", NULL));
}

void test__begins_with_whole_word_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word(uninitialized_str, "Hello", " "));
}

void test__begins_with_whole_word_ignore_case_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__begins_with_whole_word_ignore_case(str, "HeLlO", " "));

  sl_string__set(str, "Hello;World");
  TEST_ASSERT_TRUE(sl_string__begins_with_whole_word_ignore_case(str, "heLlO", ";"));
}

void test__begins_with_whole_word_ignore_case_not_found(void) {
  sl_string__set(str, "HelloWorld");
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word_ignore_case(str, "HeLlO", " "));
}

void test__begins_with_whole_word_ignore_case_unsuccessful_null_params(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word_ignore_case(str, NULL, " "));
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word_ignore_case(str, "Hello", NULL));
}

void test__begins_with_whole_word_ignore_case_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word_ignore_case(uninitialized_str, "Hello", " "));
}

void test__begins_with_whole_word_ignore_case_unsuccessful_large_substring(void) {
  sl_string__set(str, "World");
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word_ignore_case(str, "Hello World", ""));
}

void test__begins_with_whole_word_ignore_case_not_matching(void) {
  sl_string__set(str, "World");
  TEST_ASSERT_FALSE(sl_string__begins_with_whole_word_ignore_case(str, "orld", ""));
}

void test__begins_with_whole_word_ignore_case_string_delimeter_null(void) {
  sl_string__set(str, "World,");
  TEST_ASSERT_TRUE(sl_string__begins_with_whole_word_ignore_case(str, "World", "\0"));
}

void test__ends_with_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_TRUE(sl_string__ends_with(str, "World"));
}

void test__ends_with_unsuccessful_large_substring(void) {
  sl_string__set(str, "Hello");
  TEST_ASSERT_FALSE(sl_string__ends_with(str, "Hello World"));
}

void test__ends_with_unsuccessful_not_found(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__ends_with(str, "Hello"));
}

void test__ends_with_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__ends_with(str, NULL));
}

void test__ends_with_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__ends_with(uninitialized_str, "hello"));
}

void test__ends_newline_fails_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__ends_with_newline(uninitialized_str));
}

void test__ends_newline(void) {
  sl_string__set(str, "Hello World\r");
  TEST_ASSERT_TRUE(sl_string__ends_with_newline(str));
  sl_string__set(str, "Hello World\r\n");
  TEST_ASSERT_TRUE(sl_string__ends_with_newline(str));
  sl_string__set(str, "Hello World\n");
  TEST_ASSERT_TRUE(sl_string__ends_with_newline(str));
  sl_string__set(str, "Hello World");
  TEST_ASSERT_FALSE(sl_string__ends_with_newline(str));

  sl_string__set(str, "");
  TEST_ASSERT_FALSE(sl_string__ends_with_newline(str));
}

void test__ends_with_ignore_case_found(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__ends_with_ignore_case(str, "wOrLd"));
}

void test__ends_with_ignore_case_unsuccessful_large_substring(void) {
  sl_string__set(str, "Hello");

  TEST_ASSERT_FALSE(sl_string__ends_with_ignore_case(str, "Hello World"));
}

void test__ends_with_ignore_case_not_found(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__ends_with_ignore_case(str, "hELLo"));
}

void test__ends_with_ignore_case_unsuccessful_null_substring(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__ends_with_ignore_case(str, NULL));
}

void test__ends_with_ignore_case_unsuccessful_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__ends_with_ignore_case(uninitialized_str, "hey"));
}

void test__erase_successful(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase(str, "Hello"));
  TEST_ASSERT_EQUAL_STRING(" World", str.cstring);
}

void test__erase_null_substring(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__erase(str, NULL));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__erase_null_string_pointer(void) { TEST_ASSERT_FALSE(sl_string__erase(uninitialized_str, "hello")); }

void test__erase_substring_not_found(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__erase(str, "hey"));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__erase_first_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__erase_first(uninitialized_str, 4));
}

void test__erase_first_successful(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_first(str, 4));
  TEST_ASSERT_EQUAL_STRING("o World", str.cstring);
}

void test__erase_first_n_chars_longer_than_string(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_first(str, sl_string__get_length(str) + 1));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__erase_last_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__erase_last(uninitialized_str, 4));
}

void test__erase_last_successful(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_last(str, 4));
  TEST_ASSERT_EQUAL_STRING("Hello W", str.cstring);
}

void test__erase_last_n_chars_longer_than_string(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_last(str, sl_string__get_length(str) + 1));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__erase_at_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__erase_at(uninitialized_str, 6));
}

void test__erase_at_successful(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_at(str, 6));
  TEST_ASSERT_EQUAL_STRING("Hello orld", str.cstring);
}

void test__erase_at_index_out_of_bounds(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__erase_at(str, sl_string__get_length(str) + 1));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__erase_after_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__erase_after(uninitialized_str, 2, 3));
}

void test__erase_after_successful(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_after(str, 2, 3));
  TEST_ASSERT_EQUAL_STRING("He World", str.cstring);
}

void test__erase_after_index_out_of_bounds(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_FALSE(sl_string__erase_after(str, sl_string__get_length(str) + 1, 3));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);
}

void test__erase_after_n_chars_longer_than_string(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_after(str, 0, sl_string__get_length(str) + 1));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__erase_at_substring_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__erase_at_substring(uninitialized_str, " World"));
  TEST_ASSERT_FALSE(sl_string__erase_at_substring(str, NULL));
}

void test__erase_after_string(void) {
  sl_string__set(str, "Hello World 123");

  TEST_ASSERT_TRUE(sl_string__erase_at_substring(str, " World"));
  TEST_ASSERT_EQUAL_STRING("Hello", str.cstring);

  TEST_ASSERT_FALSE(sl_string__erase_at_substring(str, "none"));
  TEST_ASSERT_EQUAL_STRING("Hello", str.cstring);
}

void test__erase_first_word_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__erase_first_word(uninitialized_str, ' '));
}

void test__erase_first_word_successful(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_TRUE(sl_string__erase_first_word(str, ' '));
  TEST_ASSERT_EQUAL_STRING("World", str.cstring);
}

void test__erase_first_word_no_words(void) {
  sl_string__set(str, "HelloWorld");

  TEST_ASSERT_FALSE(sl_string__erase_first_word(str, ' '));
  TEST_ASSERT_EQUAL_STRING("HelloWorld", str.cstring);
}

void test__erase_special_chars_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_UINT16(0, sl_string__erase_special_chars(uninitialized_str));
}

void test__erase_special_chars_successful(void) {
  sl_string__set(str, "He@llo $123& Wor*ld");

  TEST_ASSERT_EQUAL_UINT16(6, sl_string__erase_special_chars(str));
  TEST_ASSERT_EQUAL_STRING("Hello123World", str.cstring);
}

void test__erase_special_chars_no_special_chars(void) {
  sl_string__set(str, "Hello123World");

  TEST_ASSERT_EQUAL_UINT16(0, sl_string__erase_special_chars(str));
  TEST_ASSERT_EQUAL_STRING("Hello123World", str.cstring);
}

void test__erase_int_unsuccessful_with_null_string_pointer(void) {
  long int result = -1;
  TEST_ASSERT_FALSE(sl_string__erase_int(uninitialized_str, &result));
}

void test__erase_int(void) {
  sl_string_s s =
      sl_string__initialize_from(string_memory, sizeof(string_memory) - string_memory_pad_size, "111.222ab333;a.444");
  long int result = -1;

  TEST_ASSERT_TRUE(sl_string__erase_int(s, &result));
  TEST_ASSERT_EQUAL_INT(111, result);

  TEST_ASSERT_TRUE(sl_string__erase_int(s, &result));
  TEST_ASSERT_EQUAL_INT(222, result);

  TEST_ASSERT_TRUE(sl_string__erase_int(s, &result));
  TEST_ASSERT_EQUAL_INT(333, result);

  TEST_ASSERT_TRUE(sl_string__erase_int(s, &result));
  TEST_ASSERT_EQUAL_INT(444, result);

  TEST_ASSERT_FALSE(sl_string__erase_int(s, &result));
  TEST_ASSERT_EQUAL_INT(444, result);
  TEST_ASSERT_FALSE(sl_string__erase_int(s, &result));
  TEST_ASSERT_EQUAL_INT(444, result);

  sl_string__set(s, "123");
  TEST_ASSERT_FALSE(sl_string__erase_int(s, NULL));
  TEST_ASSERT_EQUAL_STRING(s.cstring, "");
}

void test__trim_end_successful(void) {
  sl_string__set(str, "./././123 ***");

  TEST_ASSERT_TRUE(sl_string__trim_end(str, "./*"));
  TEST_ASSERT_EQUAL_STRING("./././123 ", str.cstring);
}

void test__trim_end_entire_string(void) {
  sl_string__set(str, "123");

  TEST_ASSERT_TRUE(sl_string__trim_end(str, "123"));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__trim_end_null_chars_to_trim(void) {
  sl_string__set(str, "./././123 ***");

  TEST_ASSERT_FALSE(sl_string__trim_end(str, NULL));
  TEST_ASSERT_EQUAL_STRING("./././123 ***", str.cstring);
}

void test__trim_end_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__trim_end(uninitialized_str, "123"));
}

void test__trim_end_chars_not_found(void) {
  sl_string__set(str, "./././123 ***");

  TEST_ASSERT_TRUE(sl_string__trim_end(str, "./"));
  TEST_ASSERT_EQUAL_STRING("./././123 ***", str.cstring);
}

void test__trim_start_successful(void) {
  sl_string__set(str, "./././123 ***");

  TEST_ASSERT_TRUE(sl_string__trim_start(str, "./*"));
  TEST_ASSERT_EQUAL_STRING("123 ***", str.cstring);

  TEST_ASSERT_TRUE(sl_string__trim_start(str, "./*"));
  TEST_ASSERT_EQUAL_STRING("123 ***", str.cstring);
}

void test__trim_start_entire_string(void) {
  sl_string__set(str, "123");

  TEST_ASSERT_TRUE(sl_string__trim_start(str, "123"));
  TEST_ASSERT_EQUAL_STRING("", str.cstring);
}

void test__trim_start_null_chars_to_trim(void) {
  sl_string__set(str, "./././123 ***");

  TEST_ASSERT_FALSE(sl_string__trim_start(str, NULL));
  TEST_ASSERT_EQUAL_STRING("./././123 ***", str.cstring);
}

void test__trim_start_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__trim_start(uninitialized_str, "123"));
}

void test__trim_start_chars_not_found(void) {
  sl_string__set(str, "./././123 ***");

  TEST_ASSERT_TRUE(sl_string__trim_start(str, "*"));
  TEST_ASSERT_EQUAL_STRING("./././123 ***", str.cstring);
}

void test__replace_first_unsuccessful_with_null_string_pointer(void) {
  sl_string__set(str, "Hello World Hello");
  TEST_ASSERT_FALSE(sl_string__replace_first(uninitialized_str, "Hello", "World"));
}

void test__replace_first_successful(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_TRUE(sl_string__replace_first(str, "Hello", "World"));
  TEST_ASSERT_EQUAL_STRING("World World Hello", str.cstring);
}

void test__replace_first_null_params(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_FALSE(sl_string__replace_first(str, NULL, "World"));
  TEST_ASSERT_FALSE(sl_string__replace_first(str, "Hello", NULL));
  TEST_ASSERT_FALSE(sl_string__replace_first(str, NULL, NULL));

  TEST_ASSERT_EQUAL_STRING("Hello World Hello", str.cstring);
}

void test__replace_first_substring_not_found(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_TRUE(sl_string__replace_first(str, "hey", "World"));
  TEST_ASSERT_EQUAL_STRING("Hello World Hello", str.cstring);
}

void test__replace_first_empty_string_and_replace_string(void) {
  sl_string__set(str, "");
  TEST_ASSERT_TRUE(sl_string__replace_first(str, "", "World"));
  TEST_ASSERT_EQUAL_STRING("World", str.cstring);
}

void test__replace_last_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__replace_last(uninitialized_str, "Hello", "World"));
}

void test__replace_last_successful(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_TRUE(sl_string__replace_last(str, "Hello", "World"));
  TEST_ASSERT_EQUAL_STRING("Hello World World", str.cstring);
}

void test__replace_last_null_params(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_FALSE(sl_string__replace_last(str, NULL, "World"));
  TEST_ASSERT_FALSE(sl_string__replace_last(str, "Hello", NULL));
  TEST_ASSERT_FALSE(sl_string__replace_last(str, NULL, NULL));

  TEST_ASSERT_EQUAL_STRING("Hello World Hello", str.cstring);
}

void test__replace_last_substring_not_found(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_TRUE(sl_string__replace_last(str, "hey", "World"));
  TEST_ASSERT_EQUAL_STRING("Hello World Hello", str.cstring);
}

void test__replace_all_successful(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_EQUAL_INT(2, sl_string__replace_all(str, "Hello", "World"));
  TEST_ASSERT_EQUAL_STRING("World World World", str.cstring);
}

void test__replace_all_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_EQUAL_INT(-1, sl_string__replace_all(uninitialized_str, "Hello", "World"));
}

void test__replace_all_null_params(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_EQUAL_INT(-1, sl_string__replace_all(str, NULL, "World"));
  TEST_ASSERT_EQUAL_INT(-1, sl_string__replace_all(str, "Hello", NULL));
  TEST_ASSERT_EQUAL_INT(-1, sl_string__replace_all(str, NULL, NULL));

  TEST_ASSERT_EQUAL_STRING("Hello World Hello", str.cstring);
}

void test__replace_all_substring_not_found(void) {
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_EQUAL_INT(0, sl_string__replace_all(str, "hey", "World"));
  TEST_ASSERT_EQUAL_STRING("Hello World Hello", str.cstring);
}

void test__replace_all_insert_too_long(void) {
  sl_string__set(str, "Hello World");

  TEST_ASSERT_EQUAL_INT(-1, sl_string__replace_all(str, "Hello", test_very_long_string));
  TEST_ASSERT_EQUAL_STRING("Hello World", str.cstring);

  // first insertion passes, 2nd insertion fails
  sl_string__set(str, "Hello World Hello");

  TEST_ASSERT_EQUAL_INT(-1, sl_string__replace_all(str, "Hello", test_very_long_string_2));
  TEST_ASSERT_EQUAL_STRING(
      "test test test test test test test test test test test test test test test test test World Hello", str.cstring);
}

void test__replace_all_double_slash_with_slash(void) {
  const char *expected_filename = "sibros/can_module/logs/debuglog__predump.in_use";
  const char *directory_name = "sibros/can_module/logs/";
  const char *preceding_name = "debuglog";
  const char *tagname = "predump";

  const int printed_count = sl_string__printf(str, "%s/%s__%s.in_use", directory_name, preceding_name, tagname);
  TEST_ASSERT_EQUAL_INT(strlen(str.cstring), printed_count);
  // replace double slash with slash from middle of the string
  TEST_ASSERT_EQUAL_INT(1, sl_string__replace_all(str, "//", "/"));
  TEST_ASSERT_EQUAL_STRING(expected_filename, str.cstring);

  const char *string_with_slash = "/sibros/can_module/logs/debuglog__predump.in_use";
  // adding slash in the beginning of string_with_slash
  sl_string__printf(str, "/%s", string_with_slash); // re-use file_name
  // replace double slash with slash from beginning of the string
  TEST_ASSERT_EQUAL_INT(1, sl_string__replace_all(str, "//", "/"));
  TEST_ASSERT_EQUAL_STRING(string_with_slash, str.cstring);

  string_with_slash = "/sibros/can_module/logs/";
  // adding slash at the end of string_with_slash
  sl_string__printf(str, "%s/", string_with_slash); // re-use file_name
  // replace double slash with slash from end of the string
  TEST_ASSERT_EQUAL_INT(1, sl_string__replace_all(str, "//", "/"));
  TEST_ASSERT_EQUAL_STRING(string_with_slash, str.cstring);
}

void test__contains_only_hex_number_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__contains_only_hex_number(uninitialized_str));
}

void test__contains_only_hex_number(void) {
  sl_string__set(str, "0x1234567890ABCDEFabcdef");
  TEST_ASSERT_TRUE(sl_string__contains_only_hex_number(str));

  sl_string__set(str, "FEDC");
  TEST_ASSERT_FALSE(sl_string__contains_only_hex_number(str));

  sl_string__set(str, "0x12FE");
  TEST_ASSERT_TRUE(sl_string__contains_only_hex_number(str));

  sl_string__set(str, "0X12FE");
  TEST_ASSERT_TRUE(sl_string__contains_only_hex_number(str));

  sl_string__set(str, "HELLO");
  TEST_ASSERT_FALSE(sl_string__contains_only_hex_number(str));

  sl_string__set(str, "0xH1F1");
  TEST_ASSERT_FALSE(sl_string__contains_only_hex_number(str));

  sl_string__set(str, "0x");
  TEST_ASSERT_FALSE(sl_string__contains_only_hex_number(str));
}

void test__contains_only_decimal_number_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__contains_only_decimal_number(uninitialized_str));
}

void test__contains_only_decimal_number(void) {
  sl_string__set(str, "1234");
  TEST_ASSERT_TRUE(sl_string__contains_only_decimal_number(str));

  sl_string__set(str, "ABCD");
  TEST_ASSERT_FALSE(sl_string__contains_only_decimal_number(str));

  sl_string__set(str, "0xABCD");
  TEST_ASSERT_FALSE(sl_string__contains_only_decimal_number(str));

  sl_string__set(str, "HELLO");
  TEST_ASSERT_FALSE(sl_string__contains_only_decimal_number(str));
}

void test__contains_only_number_unsuccessful_with_null_string_pointer(void) {
  TEST_ASSERT_FALSE(sl_string__contains_only_number(uninitialized_str));
}

void test__contains_only_number(void) {
  sl_string__set(str, "1234");
  TEST_ASSERT_TRUE(sl_string__contains_only_number(str));

  sl_string__set(str, "0xDEAD");
  TEST_ASSERT_TRUE(sl_string__contains_only_number(str));

  sl_string__set(str, "0XDEAD");
  TEST_ASSERT_TRUE(sl_string__contains_only_number(str));

  sl_string__set(str, "fecd");
  TEST_ASSERT_FALSE(sl_string__contains_only_number(str));

  sl_string__set(str, "0x123g");
  TEST_ASSERT_FALSE(sl_string__contains_only_number(str));

  sl_string__set(str, "G123");
  TEST_ASSERT_FALSE(sl_string__contains_only_number(str));
}

void test_sl_string__strnstr_returns_pointer_to_match_when_match_found(void) {
  const char *const whole_string = "whole_string_to_match";
  const sl_string_s whole_sl_string = sl_string__initialize_non_modifiable(whole_string);

  const char *const no_match_list[] = {
      "whole_string_to_match",
      "whole_string",
      "to_match",
      "c",
  };

  for (size_t match_index = 0U; match_index < ARRAY_COUNT(no_match_list); ++match_index) {
    const char *const match_string = no_match_list[match_index];

    const char *const found_string =
        sl_string__strnstr(whole_sl_string, match_string, sl_string__get_length(whole_sl_string));
    TEST_ASSERT_NOT_EQUAL(NULL, found_string);
    TEST_ASSERT_EQUAL(0, strncmp(found_string, match_string, strlen(match_string)));
  }
}

void test_sl_string__strnstr_returns_whole_string_given_empty_match_string(void) {
  const char *const whole_string = "whole_string_to_match";
  const sl_string_s whole_sl_string = sl_string__initialize_non_modifiable(whole_string);

  const char *const empty_string = "";
  TEST_ASSERT_EQUAL(whole_string,
                    sl_string__strnstr(whole_sl_string, empty_string, sl_string__get_length(whole_sl_string)));
}

void test_sl_string__strnstr_returns_null_when_match_not_found(void) {
  const char *const whole_string = "whole_string_to_match but_only_up_to_space";
  const sl_string_s whole_sl_string = sl_string__initialize_non_modifiable(whole_string);
  const size_t length_to_search = sl_string__get_length(whole_sl_string) / 2U;

  const char *const no_match_list[] = {
      // No matching characters
      "zxy",

      // First part of string matches
      "match_nothing",

      // Match not found within length to search
      " but_only",

      // Match string length too long
      "whole_string_to_match but_only_up_to_space too_long",
  };

  for (size_t match_index = 0U; match_index < ARRAY_COUNT(no_match_list); ++match_index) {
    const char *const match_string = no_match_list[match_index];

    TEST_ASSERT_EQUAL(NULL, sl_string__strnstr(whole_sl_string, match_string, length_to_search));
  }
}

void test_sl_string__strnstr_does_not_search_past_null_terminating_character(void) {
  const char whole_string[] = {'w', 'h', 'o', 'l', 'e', '\0', 's', 't', 'r', 'i', 'n', 'g'};
  const sl_string_s whole_sl_string = sl_string__initialize_non_modifiable(whole_string);
  const char *const match_string = "string";

  TEST_ASSERT_EQUAL(NULL, sl_string__strnstr(whole_sl_string, match_string, sizeof(whole_string)));
}

void test_sl_string__strnstr_returns_null_given_invalid_inputs(void) {
  const char *const whole_string = "whole_string_to_match";
  const sl_string_s whole_sl_string = sl_string__initialize_non_modifiable(whole_string);
  const char *const match_string = "to_match";

  TEST_ASSERT_EQUAL(NULL, sl_string__strnstr(whole_sl_string, match_string, 0U));
  TEST_ASSERT_EQUAL(NULL, sl_string__strnstr(whole_sl_string, NULL, sl_string__get_length(whole_sl_string)));

  sl_string_s invalid_sl_string = whole_sl_string;
  invalid_sl_string.cstring = NULL;
  TEST_ASSERT_EQUAL(NULL, sl_string__strnstr(invalid_sl_string, match_string, sl_string__get_length(whole_sl_string)));
}
