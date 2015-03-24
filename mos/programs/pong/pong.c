#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"
#include "../../stdlib/time.h"

#define UP    (char)0x48
#define DOWN  (char)0x50
#define LEFT  (char)0x4B
#define RIGHT (char)0x4D

#define LCTRL_DOWN (char)0x1D
#define RCTRL_DOWN (char)0x00
#define LCTRL_UP   (char)0x9D
#define RCTRL_UP   (char)0x00

static int PADDLE_L_RANK;
static int PADDLE_R_RANK;

static int incwidth;
static int incheight;

static int paddle_l;
static int paddle_r;
static int lscore;
static int rscore;

void print_header();

void paddle_l_up()
{
    if (paddle_l != 2)
    {
        putcharat(' ', PADDLE_L_RANK, paddle_l+1);
        paddle_l--;
        putcharat('#', PADDLE_L_RANK, paddle_l-1);
    }
}

void paddle_r_up()
{
    if (paddle_r != 2)
    {
        putcharat(' ', PADDLE_R_RANK, paddle_r+1);
        paddle_r--;
        putcharat('#', PADDLE_R_RANK, paddle_r-1);
    }
}

void paddle_l_down()
{
    if (paddle_l != incheight-2)
    {
        putcharat(' ', PADDLE_L_RANK, paddle_l-1);
        paddle_l++;
        putcharat('#', PADDLE_L_RANK, paddle_l+1);
    }
}

void paddle_r_down()
{
    if (paddle_r != incheight-2)
    {
        putcharat(' ', PADDLE_R_RANK, paddle_r-1);
        paddle_r++;
        putcharat('#', PADDLE_R_RANK, paddle_r+1);
    }
}

void draw_paddles()
{
    for(int i = 0; i < 3; i++)
    {
        putcharat('#', PADDLE_R_RANK, paddle_r-1+i);
        putcharat('#', PADDLE_L_RANK, paddle_l-1+i);
    }
}

void game_loop()
{
    paddle_l = paddle_r = incheight / 2;
    draw_paddles();
    int state = 0;
    int ticks = ticks_ms();
    int since_last = 0;

    int ball_x = incwidth*2 /3, ball_y = incheight/2, ball_vx = -1, ball_vy = 0;

    while(1)
    {
        int delta_ms = ticks_ms() - ticks;
        since_last += delta_ms;
        if (since_last > 60)
        {
            since_last = 0;
            int new_x = ball_x + ball_vx, new_y = ball_y + ball_vy;
            if(new_y < 1 || new_y > incheight-1)
            {
                ball_vy *= -1;
                new_y = ball_y + ball_vy;
            }
            if (new_x == PADDLE_R_RANK)
            {
                if (new_y == paddle_r)
                {
                    ball_vx *= -1;
                    new_x = ball_x + ball_vx;
                    ball_vy = 0;
                    new_y = ball_y + ball_vy;
                }
                else if (new_y == paddle_r+1)
                {
                    ball_vx *= -1;
                    new_x = ball_x + ball_vx;
                    ball_vy = 1;
                    new_y = ball_y + ball_vy;
                }
                else if (new_y == paddle_r-1)
                {
                    ball_vx *= -1;
                    new_x = ball_x + ball_vx;
                    ball_vy = -1;
                    new_y = ball_y + ball_vy;
                }
            }
            else if (new_x == PADDLE_L_RANK)
            {
                if (new_y == paddle_l)
                {
                    ball_vx *= -1;
                    new_x = ball_x + ball_vx;
                    ball_vy = 0;
                    new_y = ball_y + ball_vy;
                }
                else if (new_y == paddle_l+1)
                {
                    ball_vx *= -1;
                    new_x = ball_x + ball_vx;
                    ball_vy = 1;
                    new_y = ball_y + ball_vy;
                }
                else if (new_y == paddle_l-1)
                {
                    ball_vx *= -1;
                    new_x = ball_x + ball_vx;
                    ball_vy = -1;
                    new_y = ball_y + ball_vy;
                }
            }
            else if(new_x == PADDLE_L_RANK - 1)
            {
                 new_x = incwidth* 2 / 3;
                 new_y = incheight / 2;
                 ball_vx = -1;
                 ball_vy = 0;
                 rscore += 1;
                 print_header();
            }
            else if(new_x == PADDLE_R_RANK + 1)
            {
                 new_x = incwidth* 2 / 3;
                 new_y = incheight / 2;
                 ball_vx = -1;
                 ball_vy = 0;
                 lscore += 1;
                 print_header();
            }
            putcharat(' ', ball_x, ball_y);
            ball_x = new_x;
            ball_y = new_y;
            putcharat('*', ball_x, ball_y);
        }
        int c;
        while((c = getchar()) != -1)
        {
            if (state)
            {
                state = 0;
                switch(c)
                {
                    case UP:
                        paddle_r_up();
                        break;
                    case DOWN:
                        paddle_r_down();
                        break;
                }
            }
            else if (c == 'w')
            {
                paddle_l_up();
            }
            else if (c == 's')
            {
                paddle_l_down();
            }
            else if (c == 0xE0)
            {
                state = 1;
            }
        }
        ticks += delta_ms;
        if (60-delta_ms >= 10)
        {
            sleep(60-delta_ms);
        }
    }
}

void print_border()
{
    for(int i = 1 ; i < incwidth; i++)
    {
        putcharat('-', i, 0);
        putcharat('-', i, incheight);
    }
    for(int i = 1 ; i < incheight; i++)
    {
        putcharat('|', 0, i);
        putcharat('|', incwidth, i);
    }
    putcharat('$', 0, 0);
    putcharat('$', incwidth, 0);
    putcharat('$', 0, incheight);
    putcharat('$', incwidth, incheight);
}

void print_header()
{
    char header[15];
    int score1len = lscore>10?(lscore>100?2:1):0;
    sprintf(header + score1len, "%d  PONG  %d", lscore, rscore);
    for (int i = 0; header[i]; i++)
    {
        putcharat(header[i], (incwidth/2-5)+i, 0);
    }
}

void main(int argc, char** argv)
{
    incwidth = 79;
    incheight = 22;
    if (argc == 4)
    {
        incwidth = atoi(argv[2]);
        incheight = atoi(argv[3]);
        setio(atoi(argv[0]), atoi(argv[1]), incwidth, incheight);
    }
    else
        setio(0, 1, incwidth, incheight);
    PADDLE_L_RANK = 2;
    PADDLE_R_RANK = incwidth - 2;
    stdin_init();

    lscore = rscore = 0;

    print_border();
    print_header();

    game_loop();
}