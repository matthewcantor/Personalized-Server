#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "test_util.h"
#include "ll.h"

char *ll_put_lit(ll_map_t *headers, char *key, char *value) {
    return ll_put(headers, strdup(key), strdup(value));
}

void test_ll_basic_simple() {
    ll_map_t *headers = ll_init();
    assert(!ll_put_lit(headers, "hello", "goodbye"));
    assert_streq("goodbye", ll_get(headers, "hello"));
    ll_free(headers);
}

void test_ll_basic_small() {
    ll_map_t *headers = ll_init();

    assert(!ll_put_lit(headers, "software", "design"));
    assert(!ll_put_lit(headers, "adam", "blank"));
    assert(!ll_put_lit(headers, "cs", "2"));

    assert_streq("design", ll_get(headers, "software"));
    assert_streq("2", ll_get(headers, "cs"));
    assert_streq("blank", ll_get(headers, "adam"));

    char *replaced;
    replaced = ll_put_lit(headers, "software", "development");
    assert_streq("design", replaced);
    free(replaced);
    replaced = ll_put_lit(headers, "adam", "hopper");
    assert_streq("blank", replaced);
    free(replaced);
    replaced = ll_put_lit(headers, "cs", "3");
    assert_streq("2", replaced);
    free(replaced);

    assert_streq("development", ll_get(headers, "software"));
    assert_streq("hopper", ll_get(headers, "adam"));
    assert_streq("3", ll_get(headers, "cs"));

    assert(ll_get(headers, "hopper") == NULL);
    assert(ll_get(headers, "data structures") == NULL);

    ll_free(headers);
}

#define LARGE_SIZE 100
size_t count_digits(size_t number) {
    if (number == 0) return 1;
    size_t count = 0;

    if (number < 0) number *= -1;

    while (number != 0) {
        number /= 10;
        count++;
    }

    return count;
}

void test_ll_basic_large() {
    ll_map_t *headers = ll_init();

    for (size_t i = 0; i < LARGE_SIZE; i++) {
        size_t count = count_digits(i);
        char *key = malloc(count + 2); 
        char *value = malloc(count + 2); 
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "v%zu", i);
        assert(!ll_put(headers, key, value));
    }

    for (size_t i = 0; i < LARGE_SIZE; ++i) {
        size_t count = count_digits(i);
        char key[count + 2];
        char value[count + 2];
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "v%zu", i);
        assert_streq(value, ll_get(headers, key));
    }

    for (size_t i = 0; i < LARGE_SIZE; i++) {
        size_t count = count_digits(i);
        char *key = malloc(count + 2); 
        char *value = malloc(count + 2); 
        char old_value[count + 2];
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "V%zu", i);
        snprintf(old_value, count + 2, "v%zu", i);
        char *replaced = ll_put(headers, key, value);
        assert_streq(old_value, replaced);
        free(replaced);
    }

    for (size_t i = 0; i < LARGE_SIZE; ++i) {
        size_t count = count_digits(i);
        char key[count + 2];
        char value[count + 2];
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "V%zu", i);
        assert_streq(value, ll_get(headers, key));
    }

    ll_free(headers);
}

void test_ll_basic_singleton() {
    ll_map_t *headers = ll_init();
    ll_put_lit(headers, "singleton", "value");
    char *replaced;

    assert_streq("value", ll_get(headers, "singleton"));

    replaced = ll_put_lit(headers, "singleton", "newValue");
    assert_streq("newValue", ll_get(headers, "singleton"));
    assert_streq("value", replaced);
    free(replaced);

    replaced = ll_put_lit(headers, "singleton", "newnewValue");
    assert_streq("newnewValue", ll_get(headers, "singleton"));
    assert_streq("newValue", replaced);
    free(replaced);
   
    ll_free(headers);
}

void test_ll_basic_empty() {
    ll_map_t *headers = ll_init();

    assert(ll_get(headers, "key") == NULL);
    assert(ll_get(headers, "key_other") == NULL);

    ll_free(headers);
}

void test_ll_basic_gives_back_same() {
    ll_map_t *headers = ll_init();

    char *value = strdup("value");
    ll_put(headers, strdup("key"), value);
    char *value_again = ll_get(headers, "key");
    char *value_yet_again = ll_put_lit(headers, "key", "another value");
    // check for *pointer* equality rather than value equality.
    assert(value == value_again);
    assert(value == value_yet_again);
    free(value_yet_again);
    ll_free(headers);
}

void test_ll_basic() {}

void test_ll_small() {
    ll_map_t *headers = ll_init();

    assert(!ll_put_lit(headers, "software", "design"));
    assert(!ll_put_lit(headers, "adam", "blank"));
    assert(!ll_put_lit(headers, "cs", "2"));

    assert_streq("design", ll_get(headers, "software"));
    assert_streq("2", ll_get(headers, "cs"));
    assert_streq("blank", ll_get(headers, "adam"));

    char *replaced;
    replaced = ll_put_lit(headers, "software", "development");
    assert_streq("design", replaced);
    free(replaced);
    replaced = ll_put_lit(headers, "adam", "hopper");
    assert_streq("blank", replaced);
    free(replaced);
    replaced = ll_put_lit(headers, "cs", "3");
    assert_streq("2", replaced);
    free(replaced);

    assert_streq("development", ll_get(headers, "software"));
    assert_streq("hopper", ll_get(headers, "adam"));
    assert_streq("3", ll_get(headers, "cs"));

    assert(ll_get(headers, "hopper") == NULL);
    assert(ll_get(headers, "data structures") == NULL);

    strarray_t *keys = ll_get_keys(headers);
    assert_strarray_set_matches(keys, {"software", "adam", "cs"});
   
    ll_free(headers);
    
    strarray_free(keys);
}

void test_ll_large() {
    ll_map_t *headers = ll_init();

    for (size_t i = 0; i < LARGE_SIZE; i++) {
        size_t count = count_digits(i);
        char *key = malloc(count + 2); 
        char *value = malloc(count + 2); 
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "v%zu", i);
        assert(!ll_put(headers, key, value));
    }

    for (size_t i = 0; i < LARGE_SIZE; ++i) {
        size_t count = count_digits(i);
        char key[count + 2];
        char value[count + 2];
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "v%zu", i);
        assert_streq(value, ll_get(headers, key));
    }

    for (size_t i = 0; i < LARGE_SIZE; i++) {
        size_t count = count_digits(i);
        char *key = malloc(count + 2); 
        char *value = malloc(count + 2); 
        char old_value[count + 2];
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "V%zu", i);
        snprintf(old_value, count + 2, "v%zu", i);
        char *replaced = ll_put(headers, key, value);
        assert_streq(old_value, replaced);
        free(replaced);
    }

    for (size_t i = 0; i < LARGE_SIZE; ++i) {
        size_t count = count_digits(i);
        char key[count + 2];
        char value[count + 2];
        snprintf(key, count + 2, "k%zu", i);
        snprintf(value, count + 2, "V%zu", i);
        assert_streq(value, ll_get(headers, key));
    }

    strarray_t *keys = ll_get_keys(headers);
    assert(keys->length == LARGE_SIZE);

    ll_free(headers);
    
    char **expected = malloc(sizeof(char *) * LARGE_SIZE);

    for(size_t i = 0; i < LARGE_SIZE; i++){
        size_t count = count_digits(i);
        expected[i] = malloc(count + 2); 
        snprintf(expected[i], count + 2, "k%zu", i);
    }

    assert(strarray_set_matches(keys, (const char **) expected, LARGE_SIZE));

    strarray_t *exp = malloc(sizeof(strarray_t));
    exp->data = expected;
    exp->length = LARGE_SIZE;
    strarray_free(exp);

    strarray_free(keys);
}

void test_ll_singleton() {
    ll_map_t *headers = ll_init();
    ll_put_lit(headers, "singleton", "value");
    char *replaced;

    assert_streq("value", ll_get(headers, "singleton"));

    replaced = ll_put_lit(headers, "singleton", "newValue");
    assert_streq("newValue", ll_get(headers, "singleton"));
    assert_streq("value", replaced);
    free(replaced);

    replaced = ll_put_lit(headers, "singleton", "newnewValue");
    assert_streq("newnewValue", ll_get(headers, "singleton"));
    assert_streq("newValue", replaced);
    free(replaced);

    strarray_t *keys = ll_get_keys(headers);
    assert_strarray_matches(keys, {"singleton"});
   
    ll_free(headers);
    
    strarray_free(keys);
}

void test_ll_empty() {
    ll_map_t *headers = ll_init();

    assert(ll_get(headers, "key") == NULL);
    assert(ll_get(headers, "value") == NULL);
    strarray_t *keys = ll_get_keys(headers);
    assert(keys->length == 0);
    strarray_free(keys);

    ll_free(headers);
}

// void test_ll_null_input() {
//     header_ll_t *headers = ll_init();
//     ll_put_lit(headers, "key", NULL);
//     // ll_put_lit(headers, NULL, "value");

//     assert(ll_get_header(headers, "key") == NULL);
//     // assert(ll_get_header(headers, NULL) == NULL);
// }

int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    char **testnames = argv + 1;
    
    DO_TEST(test_ll_basic_simple)
    DO_TEST(test_ll_basic_small)
    DO_TEST(test_ll_basic_large)
    DO_TEST(test_ll_basic_singleton)
    DO_TEST(test_ll_basic_empty)
    DO_TEST(test_ll_basic_gives_back_same)
    DO_TEST(test_ll_basic)
    DO_TEST(test_ll_small)
    DO_TEST(test_ll_large)
    DO_TEST(test_ll_singleton)
    DO_TEST(test_ll_empty)
    // DO_TEST(test_ll_null_input)
    puts("test_ll PASS");
}

