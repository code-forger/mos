#include "malloc.h"
#include "../paging/paging.h"
#include "../io/terminal.h"
struct _alloc_t {
    unsigned long address;
    unsigned long size;        // bytes
    bool used;
    struct _alloc_t *next;             // next struct
};


typedef struct _alloc_t alloc_t;
#define BIALLOC sizeof(alloc_t)

alloc_t *alloc_curr;
alloc_t *alloc_top;

int heap = 0;


void *malloc(size_t bytes) {
    unsigned long tempsize, retaddr;
    alloc_curr = alloc_top;

    do { // loop as long as there is a next entry
        if (bytes + BIALLOC < alloc_curr->size && alloc_curr->used == false) {  // is this free and enough space?
            tempsize = alloc_curr->size;                      // save the size of the free block in a temporary variable

            alloc_curr->next = (alloc_t *) alloc_curr->address;      // point next to the next list
            alloc_curr->address = retaddr = alloc_curr->address + BIALLOC;   // before it there comes a new entry for the linked list
            alloc_curr->size = bytes;                            // put the size in size
            alloc_curr->used = true;                           // raise the used flag

            alloc_curr = alloc_curr->next;                 // create free space block
            alloc_curr->address = retaddr + bytes + BIALLOC;                    // and put the space for the linked list before it again..
            alloc_curr->size = tempsize - bytes - BIALLOC;
            alloc_curr->used = false;

            heap -= bytes + BIALLOC;                      // substract the used bytes from the heap
            return (void *) retaddr;                          // return the address
        }

        alloc_curr = alloc_curr->next;
    } while (alloc_curr != 0);

    // if we didn't succeed, ask for extra money and try it again
    return 0;
}

void init_mem() {
    int realaddr;

    paging_map_new_to_virtual(0xc0008000);

    realaddr = 0xC0008000;               // convert the page address to bytes
    alloc_curr = alloc_top = (alloc_t *) realaddr;
    alloc_curr->address = realaddr + BIALLOC;        // put the address of the new free block in address
    alloc_curr->size = 0x1000;        // put the size of it in size
    alloc_curr->used = false;                        // it is free space, so make used false

    heap += alloc_curr->size;                        // add the new space to heap
}


void free(void *address) {
    alloc_curr = alloc_top;
    unsigned long addr_deep, bAddr_deep2;
    alloc_t *alloc_curr2;

    do { // loop through the linked list
        if (alloc_curr->address == (unsigned long) address) { // the address is found
            addr_deep = alloc_curr->address + alloc_curr->size + BIALLOC; // what should the next address be (for joining them together)
            alloc_curr2 = alloc_top; // alloc_curr2 points to alloc_top
            /* try to find an address which begins at the end of the last one */
            do {    // loop through the linked list again
                if (alloc_curr2->address == addr_deep && alloc_curr2->used == false) { // does the next address come exactly after the one to free and has used = false?
                    bAddr_deep2 = true; // set the flag true
                    break; // leave the loop
                }

                alloc_curr2 = alloc_curr2->next;
            } while (alloc_curr2 != 0);

            if (bAddr_deep2 == true) { // next piece of memory comes exactly after the one to free
                alloc_curr->size = alloc_curr->next->size;
                alloc_curr->next = alloc_curr->next->next; // skip one of them, the second will get lost anyway
                alloc_curr->used = false; // set the used flag false
                // :o that's all! Was just thinking about what next.. lol..
            } else {
                alloc_curr->used = false; // just set the flag used on false...
            }
        }

        alloc_curr = alloc_curr->next;
    } while (alloc_curr != 0);
}
