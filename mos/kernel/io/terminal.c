#include "terminal.h"
#include <stdarg.h>
#include "../paging/paging.h"
#include "../kstdlib/kstdlib.h"
#include "port.h"

uint8_t terminal_make_color(terminal_color text, terminal_color back)
{
    return text | back << 4;
}

uint16_t terminal_make_character(char c, uint8_t color)
{
    uint16_t c16 = c, color16 = color;
    return c16 | color16 << 8;
}

static uint32_t VGA_WIDTH = 80;
static uint32_t VGA_HEIGHT = 25;

static char last_char_pressed;
static uint32_t current_row = 0;
static uint32_t currnet_column = 0;
static uint8_t inactive_color = COLOR_DARK_GREY | COLOR_WHITE << 4;
static uint8_t active_color = COLOR_BLACK | COLOR_WHITE << 4;
static uint16_t* terminal;
static uint16_t* process_terminal;
static uint16_t* kernel_terminal;
static uint32_t context;

#define KERNEL_CONTEXT 1
#define PROCESS_CONTEXT 0

static int32_t active_process = -1;
static int32_t last_shown;


static void push_terminal_up()
{
    for ( uint32_t y = 0; y <= VGA_HEIGHT -1; y++ )
        for ( uint32_t x = 0; x <= VGA_WIDTH; x++ )
            kernel_terminal[y * VGA_WIDTH + x] = kernel_terminal[(y + 1) * VGA_WIDTH + x];
    for (uint32_t x = 0; x <= VGA_WIDTH; x++)
        kernel_terminal[VGA_HEIGHT * VGA_WIDTH + x] = terminal_make_character(' ', active_color);
}

void push_terminal_up_at(uint32_t px, uint32_t py, uint32_t wx, uint32_t wy)
{
    for (uint32_t y = py; y < (py + wy); y++ )
        for (uint32_t x = px; x <= (px + wx); x++ )
            process_terminal[y * VGA_WIDTH + x] = process_terminal[(y + 1) * VGA_WIDTH + x];
    for (uint32_t x = px; x <= (px + wx); x++)
        process_terminal[(py + wy) * VGA_WIDTH + x] = terminal_make_character(' ', (active_process!=(int32_t)scheduler_get_pid())?inactive_color:active_color);
}

void terminal_putchar_at(char c, uint32_t x, uint32_t y)
{
    terminal_switch_context(KERNEL_CONTEXT);
    kernel_terminal[y * VGA_WIDTH + x] = terminal_make_character(c, active_color);
}

void terminal_putchar_at_for_process(char c, uint32_t x, uint32_t y)
{
    process_table_entry ptb = scheduler_get_process_table_entry(scheduler_get_pid());
    if (x <= ptb.io.wx && y <= ptb.io.wy)
        process_terminal[(y + ptb.io.py) * VGA_WIDTH + (x + ptb.io.px)] = terminal_make_character(c, (active_process!=(int32_t)scheduler_get_pid())?inactive_color:active_color);
}

void terminal_initialize()
{
    process_terminal = terminal = paging_get_terminal_buffer();
    kernel_terminal = paging_get_kernel_terminal_buffer();

    for ( uint32_t y = 0; y < VGA_HEIGHT; y++ )
        for ( uint32_t x = 0; x < VGA_WIDTH; x++ )
            kernel_terminal[y*VGA_WIDTH+x] = terminal_make_character(' ', inactive_color);

    context = PROCESS_CONTEXT;
    last_char_pressed = ' ';
    current_row = 0;
    currnet_column = 0;
    inactive_color = COLOR_DARK_GREY | (COLOR_WHITE << 4);
    active_color = COLOR_BLACK | (COLOR_WHITE << 4);
    VGA_WIDTH = 80;
    VGA_HEIGHT = 25;

    last_shown = 0;

    for ( uint32_t y = 0; y < VGA_HEIGHT; y++ )
        for ( uint32_t x = 0; x < VGA_WIDTH; x++ )
            terminal[y*VGA_WIDTH+x] = terminal_make_character(' ', inactive_color);
    //terminal_switch_context(KERNEL_CONTEXT);
}

void terminal_switch_context(uint32_t target)
{
    switch(target)
    {
    case PROCESS_CONTEXT:
        if (context == PROCESS_CONTEXT)
            break;
        kernel_terminal = paging_get_kernel_terminal_buffer();
        paging_copy_virtual_to_virtual((uint32_t)terminal, (uint32_t)kernel_terminal);
        paging_copy_virtual_to_virtual((uint32_t)process_terminal, (uint32_t)terminal);
        process_terminal = terminal;
        context = PROCESS_CONTEXT;
        break;
    default:
        if (context == KERNEL_CONTEXT)
            break;

        process_terminal = paging_get_process_terminal_buffer();
        paging_copy_virtual_to_virtual((uint32_t)terminal, (uint32_t)process_terminal);
        paging_copy_virtual_to_virtual((uint32_t)kernel_terminal, (uint32_t)terminal);
        kernel_terminal = terminal;
        context = KERNEL_CONTEXT;
    }
}

void terminal_set_color(uint8_t color)
{
    inactive_color = color;
}

void terminal_putchar(char c)
{
    if (c == '\n')
    {
        currnet_column = 0;
        if ( ++current_row == VGA_HEIGHT )
        {
            push_terminal_up();
            current_row--;
        }
    }
    else
    {
        terminal_putchar_at(c, currnet_column, current_row);
        if ( ++currnet_column == VGA_WIDTH )
        {
            currnet_column = 0;
            if ( ++current_row == VGA_HEIGHT )
            {
                push_terminal_up();
                current_row--;
            }
        }
    }
}

void terminal_string_for_process(io_part* io) // for keyboard.c
{
    /*printf("PRINTING P=%d,%d p=%d,%d w=%d,%d, c=%d, r=%d\n",
        io->pipes[0],
        io->pipes[1],
        io->px,
        io->py,
        io->wx,
        io->wy,
        io->column,
        io->row);*/
    uint8_t uc = 0;
    char c = '\0';
    while(pipe_read(io->outpipe, &uc) == 0)
    {
        c = (char)uc;

        /*if (c == 0x1A)
        {
            pipe_read(io->outpipe, &uc);
            c = (char)uc;
            uint8_t x, y;
            pipe_read(io->outpipe, &x);
            c = (char)uc;
            pipe_read(io->outpipe, &y);
            terminal_putchar_at_for_process(c, x, y);
        }*/

        if (c == '\n')
        {
            io->column = 0;
            if ( ++io->row == io->wy  + 1)
            {
                push_terminal_up_at(io->px, io->py, io->wx, io->wy);
                io->row--;
            }
        }
        else
        {
            terminal_putchar_at_for_process(c, io->column, io->row);
            if ( ++io->column == io->wx + 1)
            {
                io->column = 0;
                if ( ++io->row == io->wy + 1)
                {
                    push_terminal_up_at(io->px, io->py, io->wx, io->wy);
                    io->row--;
                }
            }
        }
    }
}

static void terminal_putint(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=10;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=10;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%10);
        buff[(length-1)-i] = out + '0';
        in/=10;
    }

    for (i--;i >= 0;i--)
        terminal_putchar(buff[(length-1)-i]);
}

static void terminal_putinthex(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=16;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=16;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%16);
        buff[(length-1)-i] = (out < 10)?(out + '0'):((out-10) + 'A');
        in/=16;
    }

    for (i--;i >= 0;i--)
        terminal_putchar(buff[(length-1)-i]);
}

static void terminal_putintbin(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=2;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=2;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%2);
        buff[(length-1)-i] = out + '0';
        in/=2;
    }

    for (i--;i >= 0;i--)
        terminal_putchar(buff[(length-1)-i]);
}

/*static void terminal_print(const char* data)
{
    uint32_t datalen = strlen(data);
    for ( uint32_t i = 0; i < datalen; i++ )
        terminal_putchar(data[i]);
}*/

void printf(const char* string, ...)
{
    if (context != 1 && context != 0)
    {
        return;
    }
    //terminal_switch_context(KERNEL_CONTEXT);
    //asm("cli");
    va_list valist;

    va_start(valist, string);

    for (uint32_t i = 0;; i++)
    {
        if(string[i] == '\0')
            break;
        if(string[i] == '%')
        {
            if(string[i+1] == '%')
                terminal_putchar('%');
            if(string[i+1] == 'd')
                terminal_putint(va_arg(valist, int));
            if(string[i+1] == 'c')
                terminal_putchar(va_arg(valist, int));
            if(string[i+1] == 'h')
                terminal_putinthex(va_arg(valist, int));
            if(string[i+1] == 'b')
                terminal_putintbin(va_arg(valist, int));
            if(string[i+1] == 's')
                printf((const char*)va_arg(valist, int));
            i++;
        }
        else
        {
            terminal_putchar(string[i]);
        }
    }
    /* clean memory reserved for valist */
    va_end(valist);
    //asm("sti");
}



static void terminal_hide_overlapping(uint32_t pid);

void terminal_setio(PIPE* pipes)
{
    process_table_entry* ptb = scheduler_get_process_table_entry_for_editing(scheduler_get_pid());
    pipe_read(pipes[1], &ptb->io.px);
    pipe_read(pipes[1], &ptb->io.py);
    pipe_read(pipes[1], &ptb->io.wx);
    pipe_read(pipes[1], &ptb->io.wy);
    ptb->io.outpipe = pipes[1];
    ptb->io.column = 0;
    ptb->io.row = 0;
    terminal_hide_overlapping(scheduler_get_pid());
}

void terminal_setin(PIPE* pipes)
{
    process_table_entry* ptb = scheduler_get_process_table_entry_for_editing(scheduler_get_pid());
    ptb->io.inpipe = pipes[0];
    ptb->flags |= F_HAS_INPUT;
    if (active_process == -1)
        active_process = scheduler_get_pid();
    else
        terminal_set_active_input(scheduler_get_pid());
}

void terminal_set_active_input(int32_t pid)
{
    process_table_entry ptb = scheduler_get_process_table_entry(active_process);
    for (uint32_t y = ptb.io.py; y <= ptb.io.py + ptb.io.wy; y++)
    {
        for (uint32_t x = ptb.io.px; x <= ptb.io.px + ptb.io.wx; x++)
        {
            char c = process_terminal[y* VGA_WIDTH + x];
            process_terminal[y * VGA_WIDTH + x] = terminal_make_character(c, inactive_color);
        }
    }

    active_process = pid;

    if (pid < 0)
    {
        return;
    }

    ptb = scheduler_get_process_table_entry(active_process);
    for (uint32_t y = ptb.io.py; y <= ptb.io.py + ptb.io.wy; y++)
    {
        for (uint32_t x = ptb.io.px; x <= ptb.io.px + ptb.io.wx; x++)
        {
            char c = process_terminal[y * VGA_WIDTH + x];
            process_terminal[y * VGA_WIDTH + x] = terminal_make_character(c, active_color);
        }
    }
}

void terminal_clear_process(uint32_t pid)
{
    process_table_entry ptb = scheduler_get_process_table_entry(pid);
    if(!(ptb.flags & F_IS_HIDDEN))
    {
        for (uint32_t y = ptb.io.py; y <= ptb.io.py + ptb.io.wy; y++)
        {
            for (uint32_t x = ptb.io.px; x <= ptb.io.px + ptb.io.wx; x++)
            {
                process_terminal[y * VGA_WIDTH + x] = terminal_make_character(' ', active_color);
            }
        }
    }
}

uint32_t terminal_get_active_input()
{
    return active_process;
}

uint32_t terminal_get_last_shown_input()
{
    return last_shown;
}

void terminal_send_to_process(char data)
{
    if (active_process != -1)
    {
        process_table_entry ptb = scheduler_get_process_table_entry(active_process);
        pipe_write(ptb.io.inpipe, data);
        scheduler_unmark_process_as(active_process, (F_PAUSED | F_SKIP));
    }
    last_char_pressed = data;
}

char terminal_get_last_char_pressed()
{
    return last_char_pressed;
}

void terminal_hide_process(uint32_t pid)
{
    if (scheduler_get_process_table_entry_for_editing(pid) != 0)
    {
        process_table_entry* ptb = scheduler_get_process_table_entry_for_editing(pid);

        if (ptb->io.snapshot)
            return;

        ptb->io.snapshot = (uint16_t*)malloc(sizeof(uint16_t) * (ptb->io.wx+1) * (ptb->io.wy+1));

        for (uint32_t y = ptb->io.py; y <= ptb->io.py + ptb->io.wy; y++)
        {
            for (uint32_t x = ptb->io.px; x <= ptb->io.px + ptb->io.wx; x++)
            {
                ptb->io.snapshot[(x - ptb->io.px) + ((y - ptb->io.py) * ptb->io.wx)] = process_terminal[y * VGA_WIDTH + x];
                process_terminal[y * VGA_WIDTH + x] = terminal_make_character(' ', active_color);
            }
        }

        scheduler_mark_process_as(pid, F_IS_HIDDEN);
    }
}

static int overlaps(uint32_t pid, uint32_t process)
{

    process_table_entry ptb1 = scheduler_get_process_table_entry(pid);
    process_table_entry ptb2 = scheduler_get_process_table_entry(process);

    return (ptb1.io.px <= ptb2.io.px + ptb2.io.wx &&
        ptb2.io.px < ptb1.io.px + ptb1.io.wx &&
        ptb1.io.py <= ptb2.io.py + ptb2.io.wy &&
        ptb2.io.py < ptb1.io.py + ptb1.io.wy);
}

static void terminal_hide_overlapping(uint32_t pid)
{
    uint32_t process = scheduler_get_next_process(0, FS_NONE, F_DEAD);
    while (process != 0)
    {
        if (process != pid && !(scheduler_get_process_table_entry(pid).flags & F_IS_HIDDEN) && overlaps(pid, process))
            terminal_hide_process(process);
        process = scheduler_get_next_process(process, FS_NONE, F_DEAD);
    }
}

void terminal_show_process(uint32_t pid)
{
    if (scheduler_get_process_table_entry_for_editing(pid) != 0)
    {
        last_shown = pid;

        scheduler_unmark_process_as(pid, F_IS_HIDDEN);

        terminal_hide_overlapping(pid);

        process_table_entry* ptb = scheduler_get_process_table_entry_for_editing(pid);

        for (uint32_t y = ptb->io.py; y <= ptb->io.py + ptb->io.wy; y++)
        {
            for (uint32_t x = ptb->io.px; x <= ptb->io.px + ptb->io.wx; x++)
            {
                process_terminal[y * VGA_WIDTH + x] = ptb->io.snapshot[(x - ptb->io.px) + ((y - ptb->io.py) * ptb->io.wx)];
            }
        }

        free(ptb->io.snapshot);
        ptb->io.snapshot = 0;
    }
}