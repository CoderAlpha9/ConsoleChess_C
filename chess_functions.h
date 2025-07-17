//----------------------prototypes------------------------
void greet();
void complain();

void reset_board(char board[8][8]);
void print_board(char board[8][8]);
void get_move(char in_arr[4], int turn); //get user input in form of 'ka1' + white(1)/black(0) turn printing

int find_piece(char board[8][8], char piece_sym, int output_coor[2]);
int is_piece(char board[8][8], char piece_sym, int x, int y); //safe access, checks if piece_sym at given pos
int reverse_locate(char board[8][8], char piece_sym, int x, int y, int output_coor[2], int skip_first); //rev search for piece from new location

int is_check(char board[8][8], char king_clr);
int can_king_move(char board[8][8], char king_clr);

int is_killable(char board[8][8], int old_x, int old_y, int new_x, int new_y);

//---------------------functions definitions-----------------------

void greet()
{
    printf("Starting game...\n\n");
    //print long stuff after implmentation
    //printf("Welcome to UniChess!\n");
    //printf("Instructions:\n");
}

//redundant
void complain()
{
    printf("Incorrect move entered!\n");
}

void reset_board(char board[8][8])
{
    char black_syms[] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'};
    char white_syms[] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

    int i, j;

    //populating boardspace

    for (j = 0; j < 8; j++)
    {
        board[7][j] = black_syms[j];
        board[6][j] = 'p';

        board[1][j] = 'P';
        board[0][j] = white_syms[j];
    }

    for (i = 2; i < 6; i++)
    {
        for (j = 0; j < 8; j++)
        {
            board[i][j] = '_';
        }
    }
}

void print_board(char board[8][8])
{
    int i, j;
    for (i = 7; i >= 0; i--) //(inverted from row access of board)
    {
        printf("%d| ", i+1); //prints row number 
        for (j = 0; j < 8; j++)
        {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
    printf("-------------------\n");
    printf("   a b c d e f g h\n"); //prints col letter
}

void get_move(char in_arr[4], int turn) //pointer to char array of size 4
{
    if (turn == 1)
    {
        printf("\n=== White's turn ===\n");
        printf("Enter your move in format 'Ka1': ");
    }
    else
    {
        printf ("\n=== Black's turn ===\n");
        printf("Enter your move in format 'kh8': ");
    }
    scanf("%3s", in_arr);
}

//sends output_coor of piece_sym on board
//returns 1 if found, 0 if not
int find_piece(char board[8][8], char piece_sym, int output_coor[2])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[j][i] == piece_sym)
            {
                output_coor[0] = i;
                output_coor[1] = j;
                return 1;
            }
        }
    }

    return 0;
}

//safe access[outofbounds handled], returns 1 is piece_sym in coor, 0 in empty sq
//                                          and returns -1 if another piece in given coor
int is_piece(char board[8][8], char piece_sym, int x, int y)
{
    if (x < 0 || x > 7 || y < 0 || y > 7) //validation
        return 0;

    if (board[y][x] != '_')
    {
        if (board[y][x] == piece_sym) 
        {
            return 1; //return 1 if required piece found
        }
        else
            return -1; //return 1 if other piece blocks search-path
    }
    else 
        return 0; //return 0 if sq is empty
}

//this function determines how pieces move -> The "heart" of this program
//starts at coor to move, and based on piece type, checks if the piece is in a place to move to that pos
//returns 1 if piece found, 0 is piece not found
//x and y are pos-to-move; output_coor stores coor of found-piece
//skip_first defaults to 0, but if skip_first = 1, the second piece of same piece_sym will move
int reverse_locate(char board[8][8], char piece_sym, int x, int y, int output_coor[2], int skip_first)
{
    int i, j; //arbitrary looping variables

    //stores arbitrary piece-found conditions
    //  => 1 if piece found, 0 if empty sq, -1 if another piece blocking search-path
    int pos_bool;

    char m = board[y][x]; //piece in to-move sq

    //white pawn -> only increase to y allowed [path-block checks needed for 2-hop]
    //              unless 1-diagonal black piece-cut
    if (piece_sym == 'P')
    {   
        pos_bool = is_piece(board, piece_sym, x, y-1);
        if (pos_bool == 1 && m=='_') //looks for piece one row down
        {
            output_coor[0] = x;
            output_coor[1] = y-1;
            return 1;
        }

        //rightward 1-diag piece cut if opponent piece in to-move sq
        else if (is_piece(board, piece_sym, x-1, y-1) == 1)
        {
            if (m=='r' || m=='n' || m=='b' || m=='q' || m=='p') //if opponent piece
            {
                output_coor[0] = x-1;
                output_coor[1] = y-1;
                return 1;
            }
        }

        //leftward 1-diag piece cut if opponent piece in to-move sq
        else if (is_piece(board, piece_sym, x+1, y-1) == 1)
        {
            if (m=='r' || m=='n' || m=='b' || m=='q' || m=='p') //if opponent piece
            {
                output_coor[0] = x+1;
                output_coor[1] = y-1;
                return 1;
            }
        }

        if (y == 3 && m=='_') //if coor to move at row 4
        {
            if (pos_bool == 0) //if row 3 empty [at y-1=3-1=2]
            {
                if (is_piece(board, piece_sym, x, y-2) == 1) //if pawn in row 2 (=> unmoved)
                {
                    output_coor[0] = x;
                    output_coor[1] = y-2; //first move of pawn can be 2 sq
                    return 1;
                }
            }
        }
    }
    
    //black pawn -> only decrease to y allowed [path-block checks needed for 2-hop]
    //              unless 1-diagonal white piece-cut
    else if (piece_sym == 'p')
    {
        pos_bool = is_piece(board, piece_sym, x, y+1);
        if (pos_bool == 1 && m=='_') //looks for piece one row up
        {
            output_coor[0] = x;
            output_coor[1] = y+1;
            return 1;
        }

        //rightward 1-diag piece cut if opponent piece in to-move sq
        else if (is_piece(board, piece_sym, x-1, y+1) == 1)
        {
            if (m=='R' || m=='N' || m=='B' || m=='Q' || m=='P') //if opponent piece
            {
                output_coor[0] = x-1;
                output_coor[1] = y+1;
                return 1;
            }
        }

        //leftward 1-diag piece cut if opponent piece in to-move sq
        else if (is_piece(board, piece_sym, x+1, y+1) == 1)
        {
            if (m=='R' || m=='N' || m=='B' || m=='Q' || m=='P') //if opponent piece
            {
                output_coor[0] = x+1;
                output_coor[1] = y+1;
                return 1;
            }
        }

        if (y == 4 && m=='_') //if coor to move at row 5
        {
            if (pos_bool == 0) //if row 6 empty [at y+1=5+1=6]
            {
                if (is_piece(board, piece_sym, x, y+2) == 1) //if pawn in row 7
                {
                    output_coor[0] = x;
                    output_coor[1] = y+2; //first move of pawn can be 2 sq (=> unmoved)
                    return 1;
                }
            }
        }
    }

    //b/w king -> moves only to adjacent squares [no path-block checks needed]
    else if (piece_sym == 'K' || piece_sym == 'k')
    {
        //loop nearby 8 sq (+1 including to-move sq, which is skipped)
        for (i = -1; i < 2; i++)
        {
            for (j = -1; j < 2; j++)
            {
                if (i==0 && j==0) //skip to-move sq
                    continue;

                else if (is_piece(board, piece_sym, x+i, y+j) == 1) //[no path-block checks needed]
                {
                    output_coor[0] = x+i;
                    output_coor[1] = y+j;
                    return 1;
                }
            }
        }
    }

    //b/w knight -> L-shaped moves; [no path-block checks needed]
    else if (piece_sym == 'N' || piece_sym == 'n')
    {
        if (is_piece(board, piece_sym, x+1, y+2) == 1)
        {
            output_coor[0] = x+1;
            output_coor[1] = y+2;
            if (skip_first == 1) {skip_first = 0;}
            else return 1;
        }

        else if (is_piece(board, piece_sym, x-1, y+2) == 1)
        {
            output_coor[0] = x-1;
            output_coor[1] = y+2;
            if (skip_first == 1) {skip_first = 0;}
            else return 1;
        }

        else if (is_piece(board, piece_sym, x+1, y-2) == 1)
        {
            output_coor[0] = x+1;
            output_coor[1] = y-2;
            if (skip_first == 1) {skip_first = 0;} 
            else return 1;
        }

        else if (is_piece(board, piece_sym, x-1, y-2) == 1)
        {
            output_coor[0] = x-1;
            output_coor[1] = y-2;
            if (skip_first == 1) {skip_first = 0;} 
            else return 1;
        }

        else if (is_piece(board, piece_sym, x+2, y+1) == 1)
        {
            output_coor[0] = x+2;
            output_coor[1] = y+1;
            if (skip_first == 1) {skip_first = 0;} 
            else return 1;
        }

        else if (is_piece(board, piece_sym, x-2, y+1) == 1)
        {
            output_coor[0] = x-2;
            output_coor[1] = y+1;
            if (skip_first == 1) {skip_first = 0;} 
            else return 1;
        }

        else if (is_piece(board, piece_sym, x+2, y-1) == 1)
        {
            output_coor[0] = x+2;
            output_coor[1] = y-1;
            if (skip_first == 1) {skip_first = 0;} 
            else return 1;
        }

        else if (is_piece(board, piece_sym, x-2, y-1) == 1)
        {
            output_coor[0] = x-2;
            output_coor[1] = y-1;
            if (skip_first == 1) {skip_first = 0;} 
            else return 1;
        }
    }

    //b/w queen and bishop -> diagonals [path-block checks needed]
    if (piece_sym == 'B' || piece_sym == 'b' || piece_sym == 'Q' || piece_sym == 'q')
    {
        for (i = 1; i < 8; i++) //diagonal ray 1 [++]
        {
            pos_bool = is_piece(board, piece_sym, x+i, y+i);
            if (pos_bool == 1)
            {
                output_coor[0] = x+i;
                output_coor[1] = y+i;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching diagonal if path-blocked by another piece
            }
        }

        for (i = 1; i < 8; i++) //diagonal ray 2 [--]
        {
            pos_bool = is_piece(board, piece_sym, x-i, y-i);
            if (pos_bool == 1)
            {
                output_coor[0] = x-i;
                output_coor[1] = y-i;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching diagonal if path-blocked by another piece
            }
        }

        for (i = 1; i < 8; i++) //diagonal ray 3 [-+]
        {
            pos_bool = is_piece(board, piece_sym, x-i, y+i);
            if (pos_bool == 1)
            {
                output_coor[0] = x-i;
                output_coor[1] = y+i;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching diagonal if path-blocked by another piece
            }
        }

        for (i = 1; i < 8; i++) //diagonal ray 4 [+-]
        {
            pos_bool = is_piece(board, piece_sym, x+i, y-i);
            if (pos_bool == 1)
            {
                output_coor[0] = x+i;
                output_coor[1] = y-i;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching diagonal if path-blocked by another piece
            }
        }
    }

    //b/w queen and rook -> straight files [path-block checks needed]
    if (piece_sym == 'R' || piece_sym == 'r' || piece_sym == 'Q' || piece_sym == 'q')
    {
        for (i = 1; i < 8; i++) //column ray to top
        {
            pos_bool = is_piece(board, piece_sym, x, y+i);
            if (pos_bool == 1) //check in same column
            {
                output_coor[0] = x;
                output_coor[1] = y+i;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching column if path-blocked by another piece
            }
        }

        for (i = 1; i < 8; i++) //column ray to bottom
        {
            pos_bool = is_piece(board, piece_sym, x, y-i);
            if (pos_bool == 1) //check in same column
            {
                output_coor[0] = x;
                output_coor[1] = y-i;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching column if path-blocked by another piece
            }
        }

        for (i = 1; i < 8; i++) //row ray to right
        {
            pos_bool = is_piece(board, piece_sym, x+i, y);
            if (pos_bool == 1) //check in same row
            {
                output_coor[0] = x+i;
                output_coor[1] = y;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching row if path-blocked by another piece
            }
        }

        for (i = 1; i < 8; i++) //row ray to left
        {
            pos_bool = is_piece(board, piece_sym, x-i, y);
            if (pos_bool == 1) //check in same row
            {
                output_coor[0] = x-i;
                output_coor[1] = y;
                if (skip_first == 1) {skip_first = 0;} 
                else return 1;
            }

            else if (pos_bool == -1)
            {
                break; //end searching row if path-blocked by another piece
            }
        }
    }

    //piece not found, all unended conditional threads converge here
    return 0;
}

//king_clr must be 'w' or 'b'
//returns 0 if no check, 1 if check
int is_check(char board[8][8], char king_clr)
{
    //         opponents
    char king, opp[4];

    if (king_clr == 'w') //white king
    {
        king = 'K';
        char arr[4] = {'r', 'n', 'b', 'q'}; //black opponents
        for (int i = 0; i < 4; i++) 
            opp[i] = arr[i]; //copy into opp[]
    }
    else //black king
    {
        king = 'k';
        char arr[4] = {'R', 'N', 'B', 'Q'}; //white opponents
        for (int i = 0; i < 4; i++) 
            opp[i] = arr[i]; //copy into opp[]
    }

    int king_pos[2]; //stores current position of king
    find_piece(board, king, king_pos); //assume king can always be found

    int dummy_arr[2];

    //if one of the opponent piece can move to king's pos, king is in check [pawn: spl case]

    for (int i = 0; i < 4; i++)
    {
        if (reverse_locate(board, opp[i], king_pos[0], king_pos[1], dummy_arr, 0) == 1)
        {
            printf("%c %d %d\n", opp[i], king_pos[0], king_pos[1]);
            return 1;
        }
    }

    //else, special diag-checks by pawns must be searched (they dont check in front)

    int x = king_pos[0], y = king_pos[1];

    if (king_clr == 'w') //white king
    {
        char pawn = 'p'; //black pawn checks white king

        //rightward 1-diag check
        if (is_piece(board, pawn, x-1, y+1) == 1)
        {
            if (board[y][x] == 'K')
            {
                return 1;
            }
        }

        //leftward 1-diag check
        else if (is_piece(board, pawn, x+1, y+1) == 1)
        {
            if (board[y][x] == 'K')
            {
                return 1;
            }
        }
    }
    else //black king
    {
        char pawn = 'P'; //white pawn checks black king

        //rightward 1-diag check
        if (is_piece(board, pawn, x-1, y-1) == 1)
        {
            if (board[y][x] == 'k')
            {
                return 1;
            }
        }

        //leftward 1-diag check
        else if (is_piece(board, pawn, x+1, y-1) == 1)
        {
            if (board[y][x] == 'k')
            {
                return 1;
            }
        }
    }

    return 0;
}

//king_clr must be 'w' or 'b'
//returns 1 if king cant move, 0 if king can move
int is_king_stuck(char board[8][8], char king_clr)
{
    char king = (king_clr == 'w') ? 'K' : 'k';

    int king_pos[2]; //stores current position of king
    find_piece(board, king, king_pos); //assume king can always be found
    
    //loop nearby 8 sq (+1 including to-move sq, which is skipped)
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i==0 && j==0) //skip to-move sq
                continue;

            else if (is_piece(board, king, king_pos[0]+i, king_pos[1]+j) == 1) //[no path-block checks needed]
            {
                return 0;
            }
        }
    }

    return 1;
}


//return 1 if piece can move to the to-move pos, else return 0
int is_killable(char board[8][8], int old_x, int old_y, int new_x, int new_y)
{
    char old_sym = board[old_y][old_x];
    char new_sym = board[new_y][new_x];

    if (new_sym == '_') //can move if new sq is empty
    {
        return 1;
    }

    //opponent piece capture

    if (old_sym=='R' || old_sym=='N' || old_sym=='B' || old_sym=='Q'|| old_sym=='K' || old_sym=='P') //pawn spl case in reverse_locate
    {
        if (new_sym=='r' || new_sym=='n' || new_sym=='b' || new_sym=='q' || new_sym=='p') //no king capture
        {
            return 1;
        }
    }

    else if (old_sym=='r' || old_sym=='n' || old_sym=='b' || old_sym=='q' || old_sym=='k' || old_sym=='p') //pawn spl case in reverse_locate
    {
        if (new_sym=='R' || new_sym=='N' || new_sym=='B' || new_sym=='Q'|| new_sym=='P') //no king capture
        {
            return 1;
        }
    }

    return 0; //all unended threads halt here
}
