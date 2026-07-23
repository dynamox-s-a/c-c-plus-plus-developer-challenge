/* Registry storage — included by exactly one translation unit. */
#include "test_framework.h"

test_entry_t _test_registry[MAX_TESTS];
int          _test_count    = 0;
int          _test_failures = 0;
const char  *_current_test  = "";

int main(void)
{
    return RUN_ALL_TESTS();
}
