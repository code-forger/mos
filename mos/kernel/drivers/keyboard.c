#include "keyboard.h"
#include "../io/terminal.h"
#include "../io/port.h"

int alpha_map[] = {-1,  -1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                  '-', '=','\b','\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
                  'i', 'o', 'p', '[', ']','\n',  -1, 'a', 's', 'd', 'f',
                  'g', 'h', 'j', 'k', 'l', ';','\'', '3',  -1,'\\', 'z', 'x',
                  'c', 'v', 'b', 'n', 'm', ',', '.', '/',  -1,  -1,  -1,
                  ' ',  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                   -1,  -1, '7', '8', '9', '-', '4', '5', '6', '+', '1',
                  '2', '3', '0', '.'};

void keyboard_interupt(void)
{
    uint8_t code = get_byte_from(0x60);
    if (code > 0 && code < 0x54 && alpha_map[code] != -1)
        terminal_send_to_process(alpha_map[code]);
}