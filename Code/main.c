#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 30
#define COLOUMNS 20
#define MAXBLOCKS ((ROWS + 2) * (COLOUMNS + 2))

#define SECONDWAIT 0
#define MSWAIT 33

#define EMPTYCELL '.'
#define FULLCELL 'O'

int PLAYGAME = 1;

#ifndef __unix__
/*  IF ON WINDOWS :  */
/*--------------------------------------*/
#include <windows.h>
#include <conio.h>
void clearBoard(void)
{
    system("cls");
}
void sleepSeconds(int seconds)
{
    long milliseconds = seconds*1000;
    Sleep(milliseconds);
}
void sleepMilliseconds(long milliseconds)
{
    Sleep(milliseconds);
}
char getch_noblock(void)
{
    char pickedup = EOF;
    if (_kbhit())
    {
        pickedup = _getch();
    }
    return pickedup;
}
/*--------------------------------------*/
#else
/*  IF ON LINUX :  */
/*--------------------------------------*/
#include <termios.h>
#include <unistd.h>
#include <time.h>
void clearBoard(void)
{
    system("clear");
}
void sleepSeconds(int seconds)
{
    sleep(seconds);
}
void sleepMilliseconds(unsigned long milliseconds)
{
    unsigned long seconds = 0;
    unsigned long millisecs = 0;
    if (milliseconds >= 1000)
    {
        seconds = (unsigned long)((milliseconds / 1000));
        millisecs = (unsigned long)((milliseconds % 1000));
    }
    else
    {
        seconds = 0;
        millisecs = milliseconds;
    }
    unsigned long nanoseconds = 1000000 * milliseconds;

    struct timespec sleepytime;
    sleepytime.tv_sec = seconds;
    sleepytime.tv_nsec = nanoseconds;

    nanosleep(&sleepytime, NULL);
}
char getch_noblock(void)
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
/*--------------------------------------*/
#endif





typedef bool flag;
typedef struct vec2
{
    int x, y;
} vec2;
typedef struct block
{
    vec2 squares[4];
    flag is_static;
    int rotation_state;
    long block_index;
} block;
typedef struct moveset
{
    flag right;
    flag left;
    flag down;
    flag rotate;
    flag smash;
} moveset;


block all_blocks[MAXBLOCKS] = {};
long blocks_count = 0;

block* add_block(block new_block)
{
    new_block.block_index = blocks_count;
    all_blocks[blocks_count] = new_block;
    blocks_count = blocks_count + 1;
    return &all_blocks[(blocks_count-1)];
}

bool checkCollision(block check_block, int check_block_index, vec2 collission_force)
{
    flag will_collide = false;
    for (int other_blocks_index = 0; other_blocks_index < blocks_count; other_blocks_index++)
    {
        if (other_blocks_index == check_block_index)
        {
            continue;
        }
        block second_block = all_blocks[other_blocks_index];
        if (second_block.is_static == false)
        {
            continue;
        }
        
        int force_x = collission_force.x;
        int force_y = collission_force.y;

        for (int chk_blk_sqr_indx = 0; chk_blk_sqr_indx < 4; chk_blk_sqr_indx++)
        {
            int main_y = check_block.squares[chk_blk_sqr_indx].y;
            int main_x = check_block.squares[chk_blk_sqr_indx].x;

            for (int scnd_blk_sqr_indx = 0; scnd_blk_sqr_indx < 4; scnd_blk_sqr_indx++)
            {
                int secondary_y = second_block.squares[scnd_blk_sqr_indx].y;
                int secondary_x = second_block.squares[scnd_blk_sqr_indx].x;

                int collission_y = main_y + force_y;
                int collission_x = main_x + force_x;
                if (collission_y == secondary_y)
                {
                    if (collission_x == secondary_x)
                    {
                        will_collide = true;
                    }
                }

            }
        }

    }
    return will_collide;
}

const vec2 gravity_force = {0, 1};
const vec2 move_right = {1, 0};
const vec2 move_left = {-1, 0};
const vec2 move_down = {0, 1};

void apply_force(vec2 force, block* last_block)
{
    // for (int block_index = 0; block_index < blocks_count; block_index += 1)
    // {
        block current_block = *last_block;
        if (current_block.is_static == true)
        {
            return;
        }
        bool will_collide = checkCollision(current_block, current_block.block_index, gravity_force);
        if (will_collide == true)
        {
            // if (force.y > 0)
            // {
                all_blocks[current_block.block_index].is_static = true;
            // }
        }
        else
        {
            for(int square_i = 0; square_i < 4; square_i += 1)
            {
                all_blocks[current_block.block_index].squares[square_i].x += force.x;
                all_blocks[current_block.block_index].squares[square_i].y += force.y;
            }
        }
    // }
}

void RotateBlock(block* target_block)
{
    block current_block = *target_block;
    int current_rotation_state = current_block.rotation_state;
    vec2 X1 = current_block.squares[1];
    if (current_rotation_state == 0)
    {
        vec2 X2 = {X1.x + 1, X1.y};
        
        vec2 sqr_1 = current_block.squares[0];
        vec2 delta_1 = {   X1.x - sqr_1.x    ,  X2.y - sqr_1.y   };
        vec2 new_sqr_1 = { X2.x - delta_1.y  ,  X2.y - delta_1.x };
        
        vec2 sqr_2 = current_block.squares[1];
        vec2 delta_2 = {   X1.x - sqr_2.x    ,  X2.y - sqr_2.y   };
        vec2 new_sqr_2 = { X2.x - delta_2.y  ,  X2.y - delta_2.x };
        
        vec2 sqr_3 = current_block.squares[2];
        vec2 delta_3 = {   X1.x - sqr_3.x    ,  X2.y - sqr_3.y   };
        vec2 new_sqr_3 = { X2.x - delta_3.y  ,  X2.y - delta_3.x };

        vec2 sqr_4 = current_block.squares[3];
        vec2 delta_4 = {   X1.x - sqr_4.x    ,  X2.y - sqr_4.y   };
        vec2 new_sqr_4 = { X2.x - delta_4.y  ,  X2.y - delta_4.x };

        (*target_block).squares[0] = new_sqr_1;
        (*target_block).squares[1] = new_sqr_2;
        (*target_block).squares[2] = new_sqr_3;
        (*target_block).squares[3] = new_sqr_4;
        (*target_block).rotation_state = 0;

        block c_b = *target_block;
    }
    else if (current_rotation_state == 1)
    {

    }
    else if (current_rotation_state == 2)
    {

    }
    else if (current_rotation_state == 3)
    {

    }
    return;
}

void MoveBlocks(moveset moves, block* last_block_pointer)
{
    vec2 movement = {0, 0};
    if (moves.right)
    {
        movement = move_right;
    }
    if (moves.left)
    {
        movement = move_left;
    }
    if (moves.down)
    {
        movement = move_down;
    }

    apply_force(movement, last_block_pointer);

    if (moves.rotate)
    {
        RotateBlock(last_block_pointer);
    }
    if (moves.smash)
    {
        
    }
    return;
}

moveset ProcessKeys(char keyboardpress)
{
    char nextKey = getch_noblock();
    if (keyboardpress == 27)
    {
        if (nextKey == EOF)
        {
            keyboardpress = 'Q';
        }
        
        if (nextKey == 91)
        {
            nextKey = getch_noblock();
        }

        if (nextKey == 65)
        {
            keyboardpress = 'W';
        }
        else if (nextKey == 66)
        {
            keyboardpress = 'S';
        }
        else if (nextKey == 67)
        {
            keyboardpress = 'D';
        }
        else if (nextKey == 68)
        {
            keyboardpress = 'A';
        }
    }

    moveset moves = {false, false, false , false};

    if (keyboardpress == 'Q' | keyboardpress == 'q')
    {
        printf("END : %d\n", keyboardpress);
        PLAYGAME = 0;
    }
    else if (keyboardpress == 'W' | keyboardpress == 'w')
    {
        // printf("ROTATE : %d\n", keyboardpress);
        moves.rotate = true;
    }
    else if (keyboardpress == 'S' | keyboardpress == 's')
    {
        // printf("DOWN : %d\n", keyboardpress);
        moves.down = true;
    }
    else if (keyboardpress == 'D' | keyboardpress == 'd')
    {
        // printf("RIGHT : %d\n", keyboardpress);
        moves.right = true;
    }
    else if (keyboardpress == 'A' | keyboardpress == 'a')
    {
        // printf("LEFT : %d\n", keyboardpress);
        moves.left = true;
    }
    else if (keyboardpress == ' ' | keyboardpress == '\n' | keyboardpress == '\r')
    {
        // printf("SMASH DOWN : %d\n", keyboardpress);
        moves.smash = true;
    }
    else
    {
        // printf("NOT RECOGNIZED : %d\n", keyboardpress);
    }

    return moves;
}



void draw(void)
{
    int cells = ROWS * (COLOUMNS+1);
    char board[cells];
    memset(board, 0, sizeof(board));
    int cell_i = 0;
    for(int row_i=0; row_i<ROWS; row_i++)
    {
        for(int coloumn_i=0; coloumn_i<COLOUMNS; coloumn_i++)
        {
            char current_cell = EMPTYCELL;
            flag is_full = false;
            for(int checking_block_index=0; checking_block_index<blocks_count;checking_block_index+=1)
            {
                block current_checking_block = all_blocks[checking_block_index];
                for (int checking_square_index=0; checking_square_index<4; checking_square_index+=1)
                {
                    vec2 checking_pos = current_checking_block.squares[checking_square_index];

                    if ((checking_pos.x == coloumn_i) && (checking_pos.y == row_i))
                    {
                        is_full = true;
                        break;
                    }
                }
                if (is_full)
                {
                    break;
                }
            }
            if (is_full)
            {
                current_cell = FULLCELL;
            }
            board[cell_i] = current_cell;
            cell_i += 1;
        }
        board[cell_i] = '\n';
        cell_i += 1;
    } 
    printf("%s\n", board);
}

void gameLoop(void)
{
    block floor_block_1, floor_block_2, floor_block_3;
    
    floor_block_1.squares[0].x = 0; floor_block_1.squares[0].y = ROWS;
    floor_block_1.squares[1].x = 1; floor_block_1.squares[1].y = ROWS;
    floor_block_1.squares[2].x = 2; floor_block_1.squares[2].y = ROWS;
    floor_block_1.squares[3].x = 3; floor_block_1.squares[3].y = ROWS;
    
    floor_block_2.squares[0].x = 4; floor_block_2.squares[0].y = ROWS;
    floor_block_2.squares[1].x = 5; floor_block_2.squares[1].y = ROWS;
    floor_block_2.squares[2].x = 6; floor_block_2.squares[2].y = ROWS;
    floor_block_2.squares[3].x = 7; floor_block_2.squares[3].y = ROWS;
    
    floor_block_3.squares[0].x = 8; floor_block_3.squares[0].y = ROWS;
    floor_block_3.squares[1].x = 9; floor_block_3.squares[1].y = ROWS;
    floor_block_3.squares[2].x = 10; floor_block_3.squares[2].y = ROWS;
    floor_block_3.squares[3].x = 11; floor_block_3.squares[3].y = ROWS;

    floor_block_1.is_static = true;
    floor_block_2.is_static = true;
    floor_block_3.is_static = true;

    block* last_block_pointer;
    last_block_pointer = add_block(floor_block_1);
    last_block_pointer = add_block(floor_block_2);
    last_block_pointer = add_block(floor_block_3);

    int game_ticks = 30;
    int tick_counter = game_ticks;
    /*
    TODO :
     reset function and R key
     ticks per second counter to automatically set sleep timers
     different types of blocks
     block rotation
     smashing down
     fix sideways collission 
    */
    do
    {
        tick_counter -= 1;
        if (tick_counter <= 0)
        {
            apply_force(gravity_force, last_block_pointer);
            tick_counter = game_ticks;
        }
        block last_block_copy = (*last_block_pointer);
        if (last_block_copy.is_static == true)
        {
            if (blocks_count < (MAXBLOCKS-5))
            {
                block new_block;
                new_block.squares[0].x = 0; new_block.squares[0].y = 1;
                new_block.squares[1].x = 1; new_block.squares[1].y = 1;
                new_block.squares[2].x = 1; new_block.squares[2].y = 0;
                new_block.squares[3].x = 2; new_block.squares[3].y = 0;
                new_block.is_static = false;
                new_block.rotation_state = 0;
                last_block_pointer = add_block(new_block);
            }
            else
            {
                printf("NO MORE BLOCKS, MAX REACHED\n");
            }
        }
        char keypress = EOF;
        keypress = getch_noblock();
        moveset to_move = ProcessKeys(keypress);
        MoveBlocks(to_move, last_block_pointer);
        
        draw();

        block c_b = *last_block_pointer;

        printf("%d , %d  |  %d , %d  |  %d , %d  |  %d , %d  \n", 
        c_b.squares[0].x , c_b.squares[0].y,
        c_b.squares[1].x , c_b.squares[1].y,
        c_b.squares[2].x , c_b.squares[2].y,
        c_b.squares[3].x , c_b.squares[3].y);
        
        sleepSeconds(SECONDWAIT);
        sleepMilliseconds(MSWAIT);
        
        clearBoard();
    }
    while(PLAYGAME == 1);
    clearBoard();
    draw();
}

void endGame(void)
{
    PLAYGAME = 0;
}

int main(int argc, char** argv)
{
    printf("START\n");
    gameLoop();
    printf("END\n");
    return 0;
}
