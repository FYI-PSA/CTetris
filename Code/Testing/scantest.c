#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <ncurses.h>

/*
int main (int argc, char **argv)
{
    char C = 0;
    while(true)
    {
        C = getchar();
        printf("%d\n", C);
    }
    return 0;
}
// */
/*
int main (int argc, char **argv)
{
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char c;
    while((c=getchar())!=27)
    {
        putchar(c);
    }
    putchar(10);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}
// */
/*
int main(int argc, char **argv)
{
    char c;
    
    initscr();
    cbreak();
    echo();
    sleep(1);
    puts("OwO");
    refresh();
    c = getch();
    putchar(c);
    refresh();
    endwin();
    return 0;
}
// */
/*
int main(int argc, char **argv)
{
    char c;
    system("/bin/stty raw");
    printf("hi");
    getchar();
    printf("bye");
    while((c=getchar()) != '.')
    {
        putchar(c);
    }
    system("/bin/stty cooked");
    return 0;
}
// */
/*
int main(int argc, char **argv)
{
    char *line = NULL;
    size_t len = 0;
    while(true)
    {
        char *data = getline(&line, &len, stdin);
        printf("%s\n", data);
        sleep(1);
    }
}
// */

/*
//#define BUFLEN ((2048 * 2048))
int main(int argc, char **argv)
{
    char BUFFER[1];
    printf("A : ");
    fflush(stdin);
    system("/bin/stty raw");
    sleep(1);
    read(1, BUFFER, 1);
    printf("%s\n", BUFFER);
    system("/bin/stty cooked");
    return 0;
}
// */

/*
int main(int argc, char **argv)
{
    initscr(); cbreak(); noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    return 0;
}
// */

/*
int main(int argc, char **argv)
{ 
    int num;
    sleep(1);
    fseek(stdin, 0, SEEK_END);
    num = ftell(stdin);
    printf("%d\n", num);
    return 0;
}
// */

/*
int main(int argc, char **argv)
{
    int ch;
    sleep(1);
    ch = getc(stdin);
    if(ch == EOF)
    {
        puts("EMPTY\n");
    }
    else
    {
        ungetc(ch, stdin);
    }
    return 0;
}
// */

// /*
// immediate mode getchar().
static int getch_lower_(int block)
{
    struct termios tc = {};
    int status;
    char rdbuf;
    // retrieve initial settings.
    if (tcgetattr(STDIN_FILENO, &tc) < 0)
        perror("tcgetattr()");
    // non-canonical mode; no echo.
    tc.c_lflag &= ~(ICANON | ECHO);
    tc.c_cc[VMIN] = block ? 1 : 0; // bytes until read unblocks.
    tc.c_cc[VTIME] = 0; // timeout.
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tc) < 0)
        perror("tcsetattr()");
    // read char.
    if ((status = read(STDIN_FILENO, &rdbuf, 1)) < 0)
        perror("read()");
    // restore initial settings.
    tc.c_lflag |= (ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &tc) < 0)
        perror("tcsetattr()");
    return (status > 0) ? rdbuf : EOF;
}

int getcharacter(void)
{
    return getch_lower_(1);
}

// return EOF if no input available.
int getch_noblock(void)
{
    return getch_lower_(0);
}

int main(int argc, char **argv)
{
    puts("SUFFER : ");
    char C;
    sleep(1);
    C = getch_noblock();
    if (C == EOF)
    {
        puts("[@] Empty.\n");
    }
    else
    {
        puts("[$] Something!\n");
    }
}

// */

