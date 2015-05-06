#include "kerneltest.h"
#include "fs/mrfs.h"
#include "fs/kmrfs.h"
#include "kstdlib/kstdlib.h"
#include "scheduler/plist.h"

int verbosity = 0;

int ktest_assert(char* test, int val, int cont)
{
    if (!val)
    {
        kprintf("FAILED : %s\n", test);
        if (!cont)
        {
            kprintf("Non-continue test Failed, halting now : ");
            kprintf(test);
            asm("cli");
            asm("hlt");
        }
        return 1;
    }
    if (verbosity)
    {
        kprintf("PASSED : %s\n", test);
    }
    return 0;
}

static int call_test(char* ruberic, uint32_t (*test_func)())
{
    kprintf("Testing : %s\n", ruberic);
    return test_func();
}

void kernel_run_cache_timing()
{
    terminal_switch_context(1);
    //paging_map_new_to_virtual(SCRATCH);
    //kmrfsNewFile("/", "a", (char*)(SCRATCH), 0x1000);

    mrfs_run_timing_test();

    asm("cli");
    asm("hlt");
}

void kernel_test_mode(uint32_t test_level, uint32_t verbin)
{

    terminal_switch_context(1);
    verbosity = verbin;

    uint32_t failures = 0;

    kprintf("#----- Running Behavioural Tests -----#\n");
    failures += call_test("malloc", malloc_behaviour_test);
    failures += call_test("mrfs", mrfs_behaviour_test);
    failures += call_test("kmrfs", kmrfs_behaviour_test);
    failures += call_test("plist", plist_behaviour_test);

    if (test_level >= TEST_LIMITS)
    {
        kprintf("#----- Running Limit Tests -----#\n");
        failures += call_test("malloc", malloc_limits_test);
        failures += call_test("mrfs", mrfs_limits_test);
        failures += call_test("kmrfs", kmrfs_limits_test);
    }

    if (test_level >= TEST_STRESS)
    {
        kprintf("#----- Running Stress Tests -----#\n");
        failures += call_test("malloc", malloc_stress_test);
        failures += call_test("mrfs", mrfs_stress_test);
        failures += call_test("kmrfs", kmrfs_stress_test);
    }

    kprintf("Tests complete with %d failures. Halting.\n", failures);

    asm("cli");
    asm("hlt");
}
