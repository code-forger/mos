#include "events.h"
#include "../paging/paging.h"
#include "../io/terminal.h"

static Node *free_list;
static Node *events;

static void insert_event(Node* node)
{
    Node *current = (Node *)events;
    Node **last = &events;
    while (current != NULL)
    {
        if (current->time > node->time)
        {
            node->next = current;
            *last = node;
            return;
        }
        else
        {
            last = &current->next;
            current = current->next;
        }
    }
    node->next = current;
    *last = node;
}

static Node* get_free_node()
{
    Node* node = free_list;
    free_list = free_list[0].next;
    node->next = NULL;
    return node;
}

static Node* insert_free_event(Node* node)
{
    node->data = 0;
    node->time = 0;
    Node *next = node->next;
    node->next = free_list;
    free_list = node;
    return next;
}

void events_init()
{
    events = NULL;

    paging_map_new_to_virtual(0xc0006000);
    free_list = paging_get_event_map();

    for(uint32_t i = 0; i < 0x1000/sizeof(Node); i++)
    {
        free_list[i].data = 0;
        free_list[i].time = 0;
        free_list[i].next = &free_list[i+1];
    }
}

void events_new_event(uint32_t data, uint64_t time)
{
    Node *node = get_free_node();
    node->data = data;
    node->time = time;
    insert_event(node);
}

int64_t events_get_event(uint64_t time)
{
    uint32_t data = 0;
    if(events && events->time < time)
    {
        data = events->data;
        events = insert_free_event(events);
        return (uint64_t)data;
    }
    else
    {
        return -1;
    }

}