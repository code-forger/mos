#include "kerneltest.h"
#include "mrfs/mrfs.h"
#include "kstdlib/kstdlib.h"

int verbosity = 0;

int ktest_assert(char* test, int val, int cont)
{
    if (!val)
    {
        printf("FAILED : %s\n", test);
        if (!cont)
        {
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

void kernel_test_mode(uint32_t test_level, uint32_t verbin)
{
    verbosity = verbin;
    printf("Kernel has entered test mode. Test will be run. Programs wont be launched\n");

    uint32_t failures = 0;

    failures += malloc_behaviour_test();
    failures += mrfs_behaviour_test();

    if (test_level >= TEST_LIMITS)
    {
        failures += malloc_limits_test();
        failures += mrfs_limits_test();
    }

    if (test_level >= TEST_STRESS)
    {
        failures += malloc_stress_test();
    }

    printf("Tests complete with %d failures. Halting.\n", failures);

    asm("cli");
    asm("hlt");
}
