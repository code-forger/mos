extern void main();

void std_init()
{
    create_heap();
    main();
    kill(get_pid());
}

asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call std_init");



