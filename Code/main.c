#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 40
#define COLOUMNS 10
#define MAXBLOCKS 106
#define EMPTYCELL '~'
#define FULLCELL '#'

#ifndef __unix__
#include <windows.h>
void clearBoard()
{
    system("cls");
}
void sleepSeconds(int seconds)
{
    int milliseconds = seconds*1000;
    Sleep(milliseconds);
}
#else
#include <unistd.h>
void clearBoard()
{
    system("clear");
}
void sleepSeconds(int seconds)
{
    sleep(seconds);
}
#endif


typedef bool flag;
typedef struct vec2
{
    int x, y;
} vec2;
typedef struct block
{
    vec2 squares[4];
} block;


block all_blocks[MAXBLOCKS] = {};
flag all_blocks_is_static[MAXBLOCKS] = {false * MAXBLOCKS};
int blocks_count = 0;

block* add_block(block new_block)
{
    all_blocks[blocks_count] = new_block;
    blocks_count = blocks_count + 1;
    return &all_blocks[(blocks_count-1)];
}

const vec2 gravity_force = {0, 1};

void gravity(void)
{
    for (int block_index = 0; block_index < blocks_count; block_index += 1)
    {
        flag wont_collide = true;
        if (all_blocks_is_static[block_index])
        {
            continue;
        }
        block main_compare_block = all_blocks[block_index];
        for (int compare_block_index = 0; compare_block_index < blocks_count; compare_block_index += 1)
        {
            if (block_index == compare_block_index)
            {
                continue;
            }
            if (!all_blocks_is_static[compare_block_index])
            {
                continue;
            }
            block comparison_block = all_blocks[block_index];
            for (int square_i = 0; square_i < 4; square_i += 1)
            {
                vec2 new_cord = main_compare_block.squares[square_i];
                new_cord.y += 1;
                for (int square_j = 0; square_j < 4; square_j += 1)
                {
                    vec2 comp_cord = main_compare_block.squares[square_j];
                    if ((new_cord.x == comp_cord.x) && (new_cord.y == comp_cord.y))
                    {
                        wont_collide = false;
                        break;
                    }
                }
                if (!wont_collide)
                {
                    break;
                }
            }
            if (!wont_collide)
            {
                break;
            }
        }
        if (wont_collide)
        {
            for(int square_i = 0; square_i < 4; square_i += 1)
            {
                all_blocks[block_index].squares[square_i].x += gravity_force.x;
                all_blocks[block_index].squares[square_i].y += gravity_force.y;
            }
        }
        else
        {
            all_blocks_is_static[block_index] = true;
            printf("a block is now static\n");
        }
    }
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

                    if((checking_pos.x == coloumn_i) && (checking_pos.y == row_i))
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
            if(is_full)
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
    int game_cycle = 200;

    block floor_block_1, floor_block_2, floor_block_3;
    
    floor_block_1.squares[0].x = 5; floor_block_1.squares[0].y = 20;
    floor_block_1.squares[1].x = 6; floor_block_1.squares[1].y = 20;
    floor_block_1.squares[2].x = 7; floor_block_1.squares[2].y = 20;
    floor_block_1.squares[3].x = 8; floor_block_1.squares[3].y = 20;
    /*
    floor_block_1.squares[0].x = 4; floor_block_1.squares[0].y = 40;
    floor_block_1.squares[1].x = 5; floor_block_1.squares[1].y = 40;
    floor_block_1.squares[2].x = 6; floor_block_1.squares[2].y = 40;
    floor_block_1.squares[3].x = 7; floor_block_1.squares[3].y = 40;
    
    floor_block_1.squares[0].x = 8; floor_block_1.squares[0].y = 40;
    floor_block_1.squares[1].x = 9; floor_block_1.squares[1].y = 40;
    floor_block_1.squares[2].x = 10; floor_block_1.squares[2].y = 40;
    floor_block_1.squares[3].x = 11; floor_block_1.squares[3].y = 40;
    */
    block* last_block = add_block(floor_block_1);
    all_blocks_is_static[0] = true;
    // add_block(floor_block_2);
    // all_blocks_is_static[1] = true;
    // add_block(floor_block_3);
    // all_blocks_is_static[2] = true;
    int frequency = 10;
    do
    {
        draw();
        gravity();
        game_cycle -= 1;
        printf("FLOOR Y LEVEL : %i\n", all_blocks[0].squares[0].y);
        printf("BLOCKS : %i\n", blocks_count);
        printf("CYCLE : %i\n", game_cycle);
        printf("SPAWN : %i\n", (game_cycle%frequency));
        printf("LAST BLOCK Y LEVEL : %i\n", (*last_block).squares[0].y);
        if (((game_cycle+12) % frequency) == 0)
        {
            block new_block;
            new_block.squares[0].x = 0; new_block.squares[0].y = 1;
            new_block.squares[1].x = 1; new_block.squares[1].y = 1;
            new_block.squares[2].x = 1; new_block.squares[2].y = 0;
            new_block.squares[3].x = 2; new_block.squares[3].y = 0;
            last_block = add_block(new_block);
        }
        sleepSeconds(1);
        clearBoard();
    }
    while (game_cycle > 0);
    clearBoard();
    draw();
}

int main(int argc, char** argv)
{
    printf("START\n");
    gameLoop();
    printf("END\n");
    return 0;
}
