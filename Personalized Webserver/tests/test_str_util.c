#include "test_util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "strarray.h"
#include "mystr.h"

const int WORD_LENGTH_BOUND = 20;

void test_strarray_new() {
    strarray_t *arr = strarray_init(20);
    assert(arr->length == 20);
    for (size_t i = 0; i < 20; i++) {
        assert(arr->data[i] == NULL);
    }
    strarray_free(arr);
}

void test_strarray_new_empty() {
    strarray_t *arr = strarray_init(0);
    assert(arr->length == 0);
    strarray_free(arr);
}

void test_strarray_new_large() {
    strarray_t *arr = strarray_init(100000);
    assert(arr->length == 100000);
    strarray_free(arr);
}

void test_strarray_free() {
    strarray_t *arr = strarray_init(20);
    for (size_t i = 0; i < 20; i++) {
        arr->data[i] = malloc(10);
    }
    strarray_free(arr);
}

void test_strarray_multiple() {
    strarray_t *arr = strarray_init(20);
    for (size_t i = 0; i < 20; i++) {
        arr->data[i] = malloc(10);
    }
    strarray_t *arr2 = strarray_init(20);
    for (size_t i = 0; i < 20; i++) {
        arr2->data[i] = malloc(10);
    }
    strarray_free(arr);
    strarray_free(arr2);
}

void test_strarray() {}

/*
Asserts every call to mystr_indexof(s, sep, i) for every 0 <= i < strlen(s)
returns the correct value.

'indices' is an ordered array of the indices of 'sep' in 's'.
*/
void assert_all_indices(size_t* indices, size_t occurrences, char* s, char sep){
    size_t length = strlen(s);
    size_t idx_pointer = 0;
    for (size_t i = 0; i < length; i++){
        if (idx_pointer == occurrences){
            assert(mystr_indexof(s, sep, i) == -1);
        }
        else {
            assert((size_t) mystr_indexof(s, sep, i) == indices[idx_pointer]);
            if (indices[idx_pointer] == i){
                idx_pointer += 1;
            }
        }
    }
}

void test_strindexof_sep_simple() {
    size_t indices[1] = {5};
    assert_all_indices(indices, sizeof(indices) / sizeof(size_t), "hello world!", ' ');

    indices[0] = 0;
    assert_all_indices(indices, sizeof(indices) / sizeof(size_t), "_gobeavers!", '_');

    indices[0] = 6;
    assert_all_indices(indices, sizeof(indices) / sizeof(size_t), "heehee.", '.');
}

void test_strindexof_missing_sep() {
    size_t empty[0] = {};
    assert_all_indices(empty, 0, "", ' ');
    assert_all_indices(empty, 0, "a", ' ');
    assert_all_indices(empty, 0, "word", ' ');
    assert_all_indices(empty, 0, "hello world!", '_');
    assert_all_indices(empty, 0, "_gobeavers!", '.');
    assert_all_indices(empty, 0, "heehee.", ' ');
}

/*
Asserts that setting start to several indices after the length of the string
returns -1.
*/
void out_of_bounds_helper(char* string, char sep){
    int length = strlen(string);
    for (int i = 0; i < 5; i++){
        assert(mystr_indexof(string, sep, length + i) == -1);
    }
}

void test_strindexof_out_of_bounds() {
    out_of_bounds_helper("", ' ');
    out_of_bounds_helper("hello world!", ' ');
    out_of_bounds_helper("_gobeavers!", '_');
    out_of_bounds_helper("heehee.", '.');
}

void test_strindexof_multiple_sep() {
    size_t i1[2] = {3, 11};
    assert_all_indices(i1, sizeof(i1) / sizeof(size_t), "www.caltech.edu", '.');

    size_t i2[5] = {0, 2, 7, 10, 15};
    assert_all_indices(i2, sizeof(i2) / sizeof(size_t), "-i-like-to-code-", '-');

    size_t i3[5] = {2, 5, 8, 12, 15};
    assert_all_indices(i3, sizeof(i3) / sizeof(size_t), "to be or not to be", ' ');

    size_t i4_slash[4] = {6, 7, 24, 29};
    size_t i4_period[2] = {11, 17};
    char *s4 = "https://sof.tware.design/24sp/";

    assert_all_indices(i4_slash, sizeof(i4_slash) / sizeof(size_t), s4, '/');
    assert_all_indices(i4_period, sizeof(i4_period) / sizeof(size_t), s4, '.');
}

void test_strindexof_lots_of_spaces(){
    size_t indices[16] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 16, 17, 18};
    assert_all_indices(indices, sizeof(indices) / sizeof(size_t), "a      b    c      ", ' ');
}

void test_strindexof_long_string(){
    size_t indices[12] = {720, 721, 722, 723, 724, 725, 1446, 1447, 1448, 1449, 1450, 1451};
    char *string = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa      bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb      cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
    assert_all_indices(indices, sizeof(indices) / sizeof(size_t), string, ' ');
}

void test_strindexof() {}

/*
Asserts that the contents of "data" match with the expected contents, along with
the lengths of the arrays.
*/
void assert_split(char expected_data[][WORD_LENGTH_BOUND], const size_t expected_length, char* string, char sep){
    strarray_t *arr = mystr_split(string, sep);
    assert(expected_length == arr->length);
    char **data = arr->data;
    for (size_t i = 0; i < expected_length; i++){
        assert(strcmp(expected_data[i], data[i]) == 0);
    }
    strarray_free(arr);
}

void test_split_simple(){
    char strings1[][WORD_LENGTH_BOUND] = {"hello", "world!"};
    assert_split(strings1, 2, "hello world!", ' ');

    char strings2[][WORD_LENGTH_BOUND] = {"gobeavers!"};
    assert_split(strings2, 1, "_gobeavers!", '_');

    char strings3[][WORD_LENGTH_BOUND] = {"heehee"};
    assert_split(strings3, 1, "heehee.", '.');
}

void test_split_missing(){
    char strings1[][WORD_LENGTH_BOUND] = {};
    assert_split(strings1, 0, "", ' ');

    char strings2[][WORD_LENGTH_BOUND] = {"hello"};
    assert_split(strings2, 1, "hello", ' ');

    char strings3[][WORD_LENGTH_BOUND] = {"hello world!"};
    assert_split(strings3, 1, "hello world!", '_');

    char strings4[][WORD_LENGTH_BOUND] = {"_gobeavers!"};
    assert_split(strings4, 1, "_gobeavers!", '.');

    char strings5[][WORD_LENGTH_BOUND] = {"heehee."};
    assert_split(strings5, 1, "heehee.", ' ');
}

void test_split_multiple(){
    char strings1[][WORD_LENGTH_BOUND] = {"www", "caltech", "edu"};
    assert_split(strings1, 3, "www.caltech.edu", '.');

    char strings2[][WORD_LENGTH_BOUND] = {"i", "like", "to", "code"};
    assert_split(strings2, 4, "-i-like-to-code-", '-');

    char strings3[][WORD_LENGTH_BOUND] = {"to", "be", "or", "not", "to", "be"};
    assert_split(strings3, 6, "to be or not to be", ' ');

    char* s4 = "https://sof.tware.design/24sp/";
    char strings4_slash[][WORD_LENGTH_BOUND] = {"https:", "sof.tware.design", "24sp"};
    char strings4_period[][WORD_LENGTH_BOUND] = {"https://sof", "tware", "design/24sp/"};
    assert_split(strings4_slash, 3, s4, '/');
    assert_split(strings4_period, 3, s4, '.');
}

void test_split_lots_of_spaces(){
    char strings[][WORD_LENGTH_BOUND] = {"a", "b", "c"};
    assert_split(strings, 3, "a      b    c      ", ' ');
}

void test_split_long_string(){
    char strings[][1000] = {"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"};
    char *string = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa      bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb      cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
    strarray_t *arr = mystr_split(string, ' ');
    size_t expected_length = 3;

    assert(expected_length == arr->length);
    char **data = arr->data;
    for (size_t i = 0; i < expected_length; i++){
        assert(strcmp(strings[i], data[i]) == 0);
    }
    strarray_free(arr);
}

void test_strsplit() {}

int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    char **testnames = argv + 1;

    DO_TEST(test_strarray_new)
    DO_TEST(test_strarray_new_empty)
    DO_TEST(test_strarray_new_large)
    DO_TEST(test_strarray_free)
    DO_TEST(test_strarray_multiple)
    DO_TEST(test_strarray)
    DO_TEST(test_strindexof_sep_simple)
    DO_TEST(test_strindexof_missing_sep)
    DO_TEST(test_strindexof_out_of_bounds)
    DO_TEST(test_strindexof_multiple_sep)
    DO_TEST(test_strindexof_lots_of_spaces)
    DO_TEST(test_strindexof_long_string)
    DO_TEST(test_strindexof)
    DO_TEST(test_split_simple)
    DO_TEST(test_split_missing)
    DO_TEST(test_split_multiple)
    DO_TEST(test_split_lots_of_spaces)
    DO_TEST(test_split_long_string)
    DO_TEST(test_strsplit)
    puts("test_str_util PASS");
}

