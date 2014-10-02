#include "scheduler.h"


void new_process(uint32_t main_address)
{
    main_address = main_address; //-Werror
    //allocate stack page
    //build basic stack (Nothing?)
    //build function call to _main
    //build iret stack call

    //add to process table at bottom
}

void kill(uint32_t PiD)
{
    PiD = PiD; // -Werror
    //set process to END FOREVER
    //Probably as simple as setting a field in the process table
}

void scheduler_time_interupt()
{
    //check current pid
    //look on process table for next process (wrap on some max pid)
    //save esp and ebp into process table
    //load esp and ebp from table
    asm("cli");
    asm("hlt");
}