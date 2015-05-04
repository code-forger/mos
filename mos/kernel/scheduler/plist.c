#include "plist.h"
#include "../io/terminal.h"

void plist_push_head(plist *list, uint32_t pid)
{
    pnode *node = malloc(sizeof(pnode));
    node->pid = pid;
    node->next = list->head;
    list->head = node;

    if (list->tail == (pnode*)0xDEAFBEEF)
        list->tail = list->head;
}

void plist_push_tail(plist *list, uint32_t pid)
{
    //kkprintf("Pushing %d\n", pid);
    if (list->tail == (pnode*)0xDEAFBEEF)
    {
        list->head = malloc(sizeof(pnode));
        list->head->pid = pid;
        list->head->next = (pnode*)0xDEAFBEEF;
        list->tail = list->head;
    }
    else
    {
        list->tail->next = malloc(sizeof(pnode));
        list->tail = list->tail->next;
        list->tail->pid = pid;
        list->tail->next = (pnode*)0xDEAFBEEF;
    }
}

int32_t plist_pop_head(plist *list)
{
    if (list->head == (pnode*)0xDEAFBEEF)
        return -1;
    {
        pnode *node = list->head;
        list->head = node->next;
        if (list->head == (pnode*)0xDEAFBEEF)
            list->tail = (pnode*)0xDEAFBEEF;
        int32_t ret = node->pid;
        free(node);
        //kkprintf("Serving %d\n", ret);
        return ret;
    }
}

uint32_t plist_remove_from(plist *list, uint32_t pid)
{
    if(list->head == (pnode*)0xDEAFBEEF)
        return 0;
    if(list->head->pid == pid)
    {
        plist_pop_head(list);
        return 1;
    }
    pnode* last = list->head;
    pnode* current = last->next;

    while(current != (pnode*)0xDEAFBEEF)
    {
        if (current->pid == pid)
        {
            if (current->next == (pnode*)0xDEAFBEEF)
                list->tail = last;
            last->next = current->next;
            free(current);
            return 1;
        }
        last = current;
        current = current->next;
    }

    return 0;
}

void plist_init_list(plist *list)
{
    list->head = list->tail = (pnode*)0xDEAFBEEF;
}

//######## ########  ######  ########  ######
//   ##    ##       ##    ##    ##    ##    ##
//   ##    ##       ##          ##    ##
//   ##    ######    ######     ##     ######
//   ##    ##             ##    ##          ##
//   ##    ##       ##    ##    ##    ##    ##
//   ##    ########  ######     ##     ######

#include "../kerneltest.h"

uint32_t plist_behaviour_test()
{
    uint32_t failures = 0;

    plist list;
    plist_init_list(&list);

    failures += ktest_assert("list head init", list.head == (pnode*)0xDEAFBEEF, ASSERT_HALT);
    failures += ktest_assert("list tail init", list.tail == (pnode*)0xDEAFBEEF, ASSERT_HALT);

    plist_push_head(&list, 1);
    plist_push_head(&list, 2);
    plist_push_head(&list, 3);

    failures += ktest_assert("list pop 321 3", plist_pop_head(&list) == 3, ASSERT_CONTINUE);
    failures += ktest_assert("list pop 321 2", plist_pop_head(&list) == 2, ASSERT_CONTINUE);
    failures += ktest_assert("list pop 321 1", plist_pop_head(&list) == 1, ASSERT_CONTINUE);
    failures += ktest_assert("list pop 321 -1", plist_pop_head(&list) == -1, ASSERT_CONTINUE);

    plist_push_tail(&list, 1);
    plist_push_tail(&list, 2);
    plist_push_tail(&list, 3);

    failures += ktest_assert("list pop 123 1", plist_pop_head(&list) == 1, ASSERT_CONTINUE);
    failures += ktest_assert("list pop 123 2", plist_pop_head(&list) == 2, ASSERT_CONTINUE);
    failures += ktest_assert("list pop 123 2", plist_pop_head(&list) == 3, ASSERT_CONTINUE);
    failures += ktest_assert("list pop 123 -1", plist_pop_head(&list) == -1, ASSERT_CONTINUE);


    return failures;
}