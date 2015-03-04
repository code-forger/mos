#include "declarations.h"

#define TEST_BEHAVIOUR 0
#define TEST_LIMITS  1
#define TEST_STRESS  2

#define ASSERT_HALT 0
#define ASSERT_CONTINUE 1

#define TEST_QUIET 0
#define TEST_VERBOSE 1

int ktest_assert(char* test, int val, int cont);

void kernel_test_mode(uint32_t test_level, uint32_t verbosity);