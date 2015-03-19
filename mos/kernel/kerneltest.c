#include "kerneltest.h"
#include "fs/mrfs.h"
#include "fs/kmrfs.h"
#include "kstdlib/kstdlib.h"

int verbosity = 0;

int ktest_assert(char* test, int val, int cont)
{
    if (!val)
    {
        printf("FAILED : %s\n", test);
        if (!cont)
        {
            printf("Non-continue test Failed, halting now.\n");
            asm("cli");
            asm("hlt");
        }
        return 1;
    }
    if (verbosity)
    {
        printf("PASSED : %s\n", test);
    }
    return 0;
}

static int call_test(char* ruberic, uint32_t (*test_func)())
{
    printf("Testing : %s\n", ruberic);
    return test_func();
}

void kernel_test_mode(uint32_t test_level, uint32_t verbin)
{
    verbosity = verbin;

    uint32_t failures = 0;

    printf("#----- Running Behavioural Tests -----#\n");
    failures += call_test("malloc", malloc_behaviour_test);
    failures += call_test("mrfs", mrfs_behaviour_test);
    failures += call_test("kmrfs", kmrfs_behaviour_test);

    if (test_level >= TEST_LIMITS)
    {
        printf("#----- Running Limit Tests -----#\n");
        failures += call_test("malloc", malloc_limits_test);
        failures += call_test("mrfs", mrfs_limits_test);
        failures += call_test("kmrfs", kmrfs_limits_test);
    }

    if (test_level >= TEST_STRESS)
    {
        printf("#----- Running Stress Tests -----#\n");
        failures += call_test("malloc", malloc_stress_test);
        failures += call_test("mrfs", mrfs_stress_test);
        failures += call_test("kmrfs", kmrfs_stress_test);
    }

    printf("Tests complete with %d failures. Halting.\n", failures);

    asm("cli");
    asm("hlt");
}
