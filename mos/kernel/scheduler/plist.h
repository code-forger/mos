#pragma once
#include "../declarations.h"
#include "../kstdlib/kstdlib.h"

typedef struct pnode
{
    uint32_t pid;
    struct pnode *next;
} pnode;

typedef struct plist
{
    pnode* head;
    pnode* tail;
} plist;

void plist_push_head(plist* list, uint32_t pid);
void plist_push_tail(plist* list, uint32_t pid);
int32_t plist_pop_head(plist* list);

uint32_t plist_remove_from(plist* list, uint32_t pid);
void plist_init_list(plist *list);

uint32_t plist_behaviour_test();