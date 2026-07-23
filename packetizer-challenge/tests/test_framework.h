/**
 * @file test_framework.h
 * @brief Minimal C unit-test framework.
 *
 * Usage:
 *   TEST(my_test) { ASSERT_EQ(1 + 1, 2); }
 *   int main(void) { return RUN_ALL_TESTS(); }
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>

/* ---- Internal registry --------------------------------------------------- */
#define MAX_TESTS 256

typedef struct { const char *name; void (*fn)(void); } test_entry_t;
extern test_entry_t _test_registry[];
extern int          _test_count;
extern int          _test_failures;
extern const char  *_current_test;

/* ---- Registration macro -------------------------------------------------- */
/* NOTE: macro parameter is 'tname' to avoid collision with struct member
 * 'name' during preprocessing. */
#define TEST(tname)                                                         \
    static void test_##tname(void);                                        \
    __attribute__((constructor))                                           \
    static void _reg_##tname(void) {                                       \
        _test_registry[_test_count].name = #tname;                        \
        _test_registry[_test_count].fn   = test_##tname;                  \
        _test_count++;                                                     \
    }                                                                      \
    static void test_##tname(void)

/* ---- Assertion macros ----------------------------------------------------- */
#define FAIL(msg) \
    do { \
        printf("  FAIL [%s]: %s (line %d)\n", _current_test, (msg), __LINE__); \
        _test_failures++; \
        return; \
    } while (0)

#define ASSERT(cond) \
    do { if (!(cond)) { FAIL(#cond " is false"); } } while (0)

#define ASSERT_EQ(a, b) \
    do { if ((a) != (b)) { \
        printf("  FAIL [%s]: %s != %s  (line %d)\n", \
               _current_test, #a, #b, __LINE__); \
        _test_failures++; return; \
    } } while (0)

#define ASSERT_NE(a, b) \
    do { if ((a) == (b)) { \
        printf("  FAIL [%s]: %s == %s  (line %d)\n", \
               _current_test, #a, #b, __LINE__); \
        _test_failures++; return; \
    } } while (0)

#define ASSERT_MEM_EQ(a, b, n) \
    do { if (memcmp((a), (b), (n)) != 0) { FAIL("memory not equal: " #a " vs " #b); } } while (0)

/* ---- Runner --------------------------------------------------------------- */
#define RUN_ALL_TESTS() _run_all_tests()

static inline int _run_all_tests(void)
{
    int passed = 0;
    for (int i = 0; i < _test_count; i++) {
        _current_test = _test_registry[i].name;
        int before = _test_failures;
        _test_registry[i].fn();
        if (_test_failures == before) {
            printf("  PASS [%s]\n", _current_test);
            passed++;
        }
    }
    printf("\n%d/%d tests passed", passed, _test_count);
    if (_test_failures > 0) {
        printf("  (%d FAILED)\n", _test_failures);
        return 1;
    }
    printf("\n");
    return 0;
}

#endif /* TEST_FRAMEWORK_H */
