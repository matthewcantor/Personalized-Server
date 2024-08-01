/** Common functions for tests. */

#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mystr.h"

/**
 * Returns whether two double values are nearly equal,
 * i.e. within 10 ** -7 of each other.
 * Floating-point math is approximate, so isclose() is preferable to ==.
 * There are some exceptions: ints (<= 53 bits) and fractions whose denominators
 * are powers of 2 (e.g. 0.5 or 0.75) can be represented exactly as a double.
 */
bool isclose(double d1, double d2);

/**
 * Returns whether two strings are equal.
 * 
 * Shorthand for strcmp(s1, s2) == 0.
 */
bool streq(const char *s1, const char *s2);
#define assert_streq(S1, S2) do { \
    char *s1 = S1; \
    char *s2 = S2; \
    if (s1 == NULL) {\
        fprintf(stderr, "%s:%d: %s: Assertion `%s != NULL` failed", __FILE__, __LINE__, __func__, #S1); \
        abort(); \
    } \
    if (s2 == NULL) {\
        fprintf(stderr, "%s:%d: %s: Assertion `%s != NULL` failed", __FILE__, __LINE__, __func__, #S2); \
        abort(); \
    } \
    if (!streq(s1, s2)) { \
        fprintf(stderr, "%s:%d: %s: Assertion `streq(%s, %s)` failed: `%s` != `%s`\n", __FILE__, __LINE__, __func__, #S1, #S2, s1, s2); \
        abort(); \
    } \
} while (0);

/**
 * Returns whether two string arrays are equal.
 * 
 * Two string arrays are equal if they have the same length and each element
 * at the same index is equal.
 */
bool strarray_eq(strarray_t *arr1, strarray_t *arr2);
#define assert_strarray_eq(arr1, arr2) assert(strarray_eq(arr1, arr2))

/**
 * Returns whether a strarray is equal to an array of strings.
 */
bool strarray_matches(strarray_t *arr, const char *expected[], size_t length);
/**
 * Returns whether a strarray matches an array of strings as a set, i.e., if the
 * elements are the same, regardless of order. This is an O(n log n) function.
 */
bool strarray_set_matches(strarray_t *arr, const char *expected[], size_t length);
/**
 * Asserts that a strarray is equal to an array of strings, passed as a literal
 * array of strings.
 */
#define assert_strarray_matches(arr, ...) assert(strarray_matches(arr, (const char *[])__VA_ARGS__, sizeof((const char *[])__VA_ARGS__) / sizeof(const char *)))
/**
 * Asserts that a strarray has the same strings as an array of strings, 
 * passed as a literal array of strings.
 */
#define assert_strarray_set_matches(arr, ...) assert(strarray_set_matches(arr, (const char *[])__VA_ARGS__, sizeof((const char *[])__VA_ARGS__) / sizeof(const char *)))

/**
 * Returns a strarray_t from an array of strings, duplicating the strings.
 * 
 * The number of strings must be passed as the second argument.
 */
strarray_t *strarray_from_array(const char *arr[], size_t length);

/**
 * Returns a strarray_t from a variable number of strings, duplicating the strings.
 */
#define STRARRAY_LIT(...) (strarray_from_array((const char *[]){__VA_ARGS__}, sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *)))

/**
 * Open the file 'filename', read one word into 'testname', and close the file.
 * If the file cannot be found, exit with error.
 */
void read_testname(char *filename, char *testname, size_t testname_size);

/**
 * Checks whether any element of the NULL-terminated array `testnames` occurs
 * in the string `test`.
 */
bool contains_any(char **testnames, char *test);

/*
 * This macro checks whether to run the test function (which will be true
 * if the test is called without command-line arguments).
 * Then, it calls the test function, and prints "<test-func> PASS".
 *
 * The macro can simply be called as
 *      DO_TEST(test_func_name)
 * It doesn't matter if 'test_func_name' is a valid variable, because
 * the macro is a simple text replacement done *before* compilation.
 * No ; is needed after the closing ), because again, the macro is a simple
 * text replacement, and the statements already have ;s.
 */
#define DO_TEST(TEST_FN, ...) \
if (all_tests || contains_any(testnames, #TEST_FN)) { \
    TEST_FN(__VA_ARGS__); \
    puts(#TEST_FN " PASS"); \
}

/**
 * Executes function 'run' and returns whether it causes an assertion failure,
 * as detected by a SIGABRT signal.
 * Passes the auxiliary argument 'aux' to 'run', i.e. calls run(aux).
 */
bool test_assert_fail(void (*run)(void *aux), void *aux);

#endif // #ifndef __TEST_UTIL_H__

