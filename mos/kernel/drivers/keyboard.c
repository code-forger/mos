#include "keyboard.h"

// Convenience maps from scan codes to characters.
static const int alpha_map[] = {-1,  -1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                               '-', '=','\b','\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
                               'i', 'o', 'p', '[', ']','\n',  -1, 'a', 's', 'd', 'f',
                               'g', 'h', 'j', 'k', 'l', ';','\'', '#',  -1,'\\', 'z', 'x',
                               'c', 'v', 'b', 'n', 'm', ',', '.', '/',  -1,  -1,  -1,
                               ' ',  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                                -1,  -1, '7', '8', '9', '-', '4', '5', '6', '+', '1',
                               '2', '3', '0', '.'};

static const int upper_alpha_map[] = {-1,  -1, '!', '"', '!', '$', '%', '^', '&', '*', '(', ')',
                                     '_', '+','\b','\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
                                     'I', 'O', 'P', '{', '}','\n',  -1, 'A', 'S', 'D', 'F',
                                     'G', 'H', 'J', 'K', 'L', ':','@', '~',  -1,'|', 'Z', 'X',
                                     'C', 'V', 'B', 'N', 'M', '<', '>', '?',  -1,  -1,  -1,
                                     ' ',  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                                      -1,  -1, '7', '8', '9', '-', '4', '5', '6', '+', '1',
                                     '2', '3', '0', '.'};

// Special codes for modifier keys
static const int LSHIFT_DOWN = 0x2A;
static const int RSHIFT_DOWN = 0x36;
static const int LSHIFT_UP = 0xAA;
static const int RSHIFT_UP = 0xB6;
static const int LCTRL_DOWN = 0x1D;
static const int RCTRL_DOWN = 0x00;
static const int LCTRL_UP = 0x9D;
static const int RCTRL_UP = 0x00;
static const int F1_DOWN = 0x3B;
static const int F2_DOWN = 0x3C;

// Flags for modifier keys
static bool shift;
static bool ctrl;
static bool command;

// Ask the keyboard buffer for one character.
char keyboard_get_a_byte()
{
    return alpha_map[get_byte_from(0x60)];
}

void keyboard_init()
{
    shift = 0;
    ctrl = 0;
    command = 0;
}

void keyboard_interupt(void)
{
    uint8_t code = get_byte_from(0x60);
    if (command) // Previous code was 0xE0 indicating incoming 2 byte sequence.
    {
        command = 0;
        terminal_send_to_process(code);
    }
    // If code is in map range and map has entry.
    else if (code > 0 && code < 0x54 && alpha_map[code] != -1)
        if (ctrl && alpha_map[code] == '\t')       // ctrl+tab pressed.
            terminal_switch_input();
        else if (shift && alpha_map[code] == '\t') // shift+tab pressed.
            terminal_switch_hidden();
        else
            terminal_send_to_process((!shift)?alpha_map[code]:upper_alpha_map[code]); // Send character pressed to process.
    else if (code == 0xE0) // This special code means next byte received is NOT a normal character.
    {
        command = 1;
        terminal_send_to_process(code);
    }
    else if (code == LSHIFT_DOWN || code == RSHIFT_DOWN)  // Check for all modifier codes:
    {
        shift = 1;
        //terminal_send_to_process(code);
    }
    else if  (code == LSHIFT_UP || code == RSHIFT_UP) // --
    {
        shift = 0;
        //terminal_send_to_process(code);
    }
    else if (code == LCTRL_DOWN || code == RCTRL_DOWN) // --
    {
        ctrl = 1;
        terminal_send_to_process(code);
    }
    else if  (code == LCTRL_UP || code == RCTRL_UP) // --
    {
        ctrl = 0;
        terminal_send_to_process(code);
    }
    // Special case to switch terminal context
    else if (code == F1_DOWN)
        terminal_switch_context(0);
    else if (code == F2_DOWN)
        terminal_switch_context(1);
}