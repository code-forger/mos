#include "keyboard.h"
#include "../io/terminal.h"
#include "../io/port.h"

int alpha_map[] = {-1,  -1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                  '-', '=','\b','\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
                  'i', 'o', 'p', '[', ']','\n',  -1, 'a', 's', 'd', 'f',
                  'g', 'h', 'j', 'k', 'l', ';','\'', '#',  -1,'\\', 'z', 'x',
                  'c', 'v', 'b', 'n', 'm', ',', '.', '/',  -1,  -1,  -1,
                  ' ',  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                   -1,  -1, '7', '8', '9', '-', '4', '5', '6', '+', '1',
                  '2', '3', '0', '.'};

int upper_alpha_map[] = {-1,  -1, '!', '"', '!', '$', '%', '^', '&', '*', '(', ')',
                  '_', '+','\b','\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
                  'I', 'O', 'P', '{', '}','\n',  -1, 'A', 'S', 'D', 'F',
                  'G', 'H', 'J', 'K', 'L', ':','@', '~',  -1,'|', 'Z', 'X',
                  'C', 'V', 'B', 'N', 'M', '<', '>', '?',  -1,  -1,  -1,
                  ' ',  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                   -1,  -1, '7', '8', '9', '-', '4', '5', '6', '+', '1',
                  '2', '3', '0', '.'};

#define LSHIFT_DOWN 0x2A
#define RSHIFT_DOWN 0x36
#define LSHIFT_UP 0xAA
#define RSHIFT_UP 0xB6
#define LCTRL_DOWN 0x1D
#define RCTRL_DOWN 0x00
#define LCTRL_UP 0x9D
#define RCTRL_UP 0x00
#define F1_DOWN 0x3B
#define F2_DOWN 0x3C

static bool shift = false;
static bool ctrl = false;
static bool command = false;

char keyboard_get_a_byte()
{
    return alpha_map[get_byte_from(0x60)];
}

void keryboard_init()
{
    shift = false;
    ctrl = false;
    command = false;
}

void keyboard_interupt(void)
{
    uint8_t code = get_byte_from(0x60);
    if (command)
    {
        command = false;
        terminal_send_to_process(code);
    }
    else if (code > 0 && code < 0x54 && alpha_map[code] != -1)
        if (ctrl && alpha_map[code] == '\t')
        {
            int32_t target;
            if ((target = scheduler_get_next_process(terminal_get_active_input(), F_HAS_INPUT, F_DEAD)) >= 0)
                terminal_set_active_input(target);
        }
        else if (shift && alpha_map[code] == '\t')
        {
            int32_t target;
            if ((target = scheduler_get_next_process(terminal_get_last_shown_input(), F_IS_HIDDEN, F_DEAD)) >= 0)
                terminal_show_process(target);
        }
        else
        {
            terminal_send_to_process((!shift)?alpha_map[code]:upper_alpha_map[code]);
        }
    else if (code == 0xE0)
    {
        command = true;
        terminal_send_to_process(code);
    }
    else if (code == LSHIFT_DOWN || code == RSHIFT_DOWN)
        shift = true;
    else if  (code == LSHIFT_UP || code == RSHIFT_UP)
        shift = false;
    else if (code == LCTRL_DOWN || code == RCTRL_DOWN)
        ctrl = true;
    else if  (code == LCTRL_UP || code == RCTRL_UP)
        ctrl = false;
    else if (code == F1_DOWN)
        terminal_switch_context(0);
    else if (code == F2_DOWN)
        terminal_switch_context(1);
}