/*
Reference: Codes for pieces and colors
    //wh - white, bl - black

    wh, bl            [Capital letter = white piece, small letter = black piece]
    K , k - king
    Q , q - queen
    B , b - bishop
    N , n - knight
    R , r - rook
    P , p - pawn

    _ for empty cell

    board numbering -> letters for rows [a-h left to right] and numbers for columns [1-8 top to bottom]
                                -conv to 1-8 internally-

    NOTE: access like boardspace[y][x], 
            where x from letters ('a' in "ka1") 
                and y from numbers ('1' in "ka1")

    Acronyms used:
        "coor" : coordinates
        "rev"  : reverse
        "sq"   : square
*/

#include <stdio.h>
#include "chess_fucntions.h"

//-----------------------main----------------------------
void main()
{
    //variable declarations

    char black_pieces[9] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r', 'p'};
    char white_pieces[9] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R', 'P'};
    
    int i = 0, j = 0; //arbitrary looping vars
    int flag = 0; //arbitrary flag

    char boardspace[8][8]; //contains the entire board
    int turn = 1; // 1 if white and 0 is black

    char inp[4]; //array to capture user input (for moves) of len 3
    int output_coor[2]; //to get output from search_pos
    int output_coor_2[2]; //to get output in case of multi-piece movability

    int x, y; //arbitrarily hold x,y coordinates
    int print_board_flag = 1; //says if board must be printed in the next iteration

    //defining boardspace for first time

    reset_board(boardspace); //setting up the board for first time

    //program mainloop

    greet();

    while (1)
    {
        if (print_board_flag)
            print_board(boardspace); //printing intial layout to screen
        else
            print_board_flag = 1; //reset printing option if not already on

        get_move(inp, turn); //get user input for move, eg: "ka1"
        //inp[0] = char piece_sym, inp[1] = char letter coor for row, inp[2] = char num coor for col

        x = inp[1] - 97; //97 --> 'a'   // cols
        y = inp[2] - 49; //49 --> '1'   // rows

        //ensure that x,y are in 0-7 range
        if ((x<0 || x>7) || (y<0 || y>7))
        {
            //complain();
            printf("Given position not in board range!\nPlease try again.\n");
            print_board_flag = 0;
            continue;
        }

        //temp feature shutdown
        /*
        //ensure piece is in symbol list and impose white/black turntaking
        flag = 0;
        for (i = 0; i < 9; i++)
        {
            if (turn == 1 && inp[0] == white_pieces[i]) //white's turn
            {
                flag = 1;
                break;
            }

            else if (turn == 0 && inp[0] == black_pieces[i]) //black's turn
            {
                flag = 1;
                break;
            }
        }
        if (!flag)
        {
            //complain();
            printf("Piece either does not exist or is of opposite colour!\nPlease try again.\n");
            print_board_flag = 0;
            continue;
        }
        */

        flag = reverse_locate(boardspace, inp[0], x, y, output_coor, 0);
        //if piece not found, complain
        if (flag == 0)
        {
            //complain();
            printf("Incorrect move entered!\nPlease try again.\n");
            print_board_flag = 0;
            continue;
        }

        // flag = 1 hereafter, one piece has been found

        //rook and knight are orig multi, queen and bishop can be via promotion
        //implementing multi piece possibiltity for same to-pos (ask user to choose)
        if (inp[0]=='r' || inp[0]=='n' || inp[0]=='b' || inp[0]=='q' || inp[0]=='R' || inp[0]=='N' || inp[0]=='B' || inp[0]=='Q')
        {
            //output_coor_2 declared previously for use here

            if (reverse_locate(boardspace, inp[0], x, y, output_coor_2, 1) == 1) //search 2nd piece movability
            {
                printf("\nWhich %c piece do you want to move?\n", inp[0]);
                //                                                                    +97 for converting to charater from ord('a'); +1 for [0-7]=>[1-8]
                printf("Enter '1' for piece at (%c%d) and '2' for (%c%d): ", output_coor[0]+97, output_coor[1]+1, output_coor_2[0]+97, output_coor_2[1]+1);
                
                int choice;
                scanf("%1d", &choice);
                printf("\n");

                //if choice is 1 correct output_coor already stored, no action taken

                //if choice is 2 (the other piece) output_coor_2 is copied onto
                //                                 output_coor for further use
                if (choice == 2)
                {
                    output_coor[0] = output_coor_2[0];
                    output_coor[1] = output_coor_2[1];
                }

            }
        }

        /*  after this point output_coor is assumed to have valid old coor of piece about to move,
            and x, y are assumed to hold valid coor to move this piece to.
        */

        //if piece already there in the new coor, complain
        //change
        if (!is_killable(boardspace, output_coor[0], output_coor[1], x, y))
        {
            //complain();
            printf("Invalid move/capture!\nPlease try again.\n");
            print_board_flag = 0;
            continue;
        }

        boardspace[y][x] = inp[0]; //move the piece to new coor
        boardspace[output_coor[1]][output_coor[0]] = '_'; //clear the old coor
        
        printf("Moved: %s\n", inp);

        // /*
        if (is_check(boardspace, 'w'))
        {
            if (is_king_stuck(boardspace, 'w'))
            {
                printf("\n\nWhite Checkmated.\nBlack Wins.\n");
                //break; //Turned off checkmate ending
            }
            else
            {
                printf("\nWhite is in CHECK!\n\n");
            }
        }
        else if (is_check(boardspace, 'b'))
        {
            if (is_king_stuck(boardspace, 'b'))
            {
                printf("\n\nBlack Checkmated.\nWhite Wins.\n\n");
                //break; //Turned off checkmate ending
            }
            else
            {
                printf("\nBlack is in CHECK!\n\n");
            }
        }
        // */

        turn = 1 - turn; //0->1 and 1->0
    }

    print_board(boardspace);
    printf("\nEnding program...");
}


//implement white and black turns - turned off
//pawn promotion
//castling move and eligibility
//-- implement check and checkmate - turned off, how to say a move to prevent checkmate?
//-- implement en
//appropriate error messages
//change abs function