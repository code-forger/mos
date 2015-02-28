#include "kerneltest.h"
#include "mrfs/mrfs.h"
#include "kstdlib/kstdlib.h"

void kernel_test_mode()
{
    printf("Kernel has entered test mode. Test will be run. Programs wont be launched\n");

    malloc_selftest();

    mrfs_selftest();

    printf("Tests complete. Halting.\n");

    asm("cli");
    asm("hlt");
}
