#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <curses.h>
#include <ncurses.h>
#include <termios.h>

int getchNoBlock(void)
{
    struct termios terminal_current = {};
    int readstatus;
    int readbuffer;

    if (tcgetattr(STDIN_FILENO, &terminal_current) < 0)
    {
        perror("tcgetattr()");
    }

    terminal_current.c_lflag = ((terminal_current.c_lflag) & (~(ICANON | ECHO)));
    terminal_current.c_cc[VMIN] = 0;
    terminal_current.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_current) < 0)
    {
        perror("tcsetattr()");
    }

    readstatus = read(STDIN_FILENO, &readbuffer, 1);
    if (readstatus < 0)
    {
        perror("read()");
    }

    terminal_current.c_lflag = ((terminal_current.c_lflag) | ((ICANON | ECHO)));

    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &terminal_current) < 0)
    {
        perror("tcsetattr()");
    }
    
    if (readstatus > 0)
    {
        return readbuffer;
    }
    else
    {
        return EOF;
    }
}

int main (int argc, char **argv)
{
    return 0;
}
