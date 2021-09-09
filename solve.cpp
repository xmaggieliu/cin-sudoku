//-------------------------------------------------------------------------------------------------------------------->

//TESTED CASES:

// Solved by elimination (logic restraints)
// 1 (30 DIG): 530070000600195000098000060800060003400803001700020006060000280000419005000080079

// Solved with elimination + trials (no backtracking needed)
// 2 (29 DIG): 507200090006030701400000060100490007000508000800027005070000009209080600040000308
// 3 (28 DIG): 003005260005070000170002040000207010400000008090308000050900074000060800084700100

// Backtracking used
// 4 (39 DIG): 000074316000603840000008500725800034000030050000002798008940000040085900971326485
// 5 (17 DIG): 000801000000000043500000000000070800000000100020030000600000075003400000000200600
// 6 (20 DIG): 000000200080007090602000500070060000000901000000020040005000603090400070006000000
// 7 (17 DIG): 084000000000201000000300600000090080300000050200000000070000103000540000000000200

// Error
// 8 -invalid: 111111111100000000000000000000000000000000000000000000000000000000000000000000000

// Asks for another input
// 9: hello world!
// 10: 084000000000201000000300600000090080300000050200000000070000103000540000000000XXX
// 11: 314159265358979

//-------------------------------------------------------------------------------------------------------------------->

#include <iostream>
#include <string>

using namespace std;

// Size of Sudoku
#define n 9


//-------------------------------------------->

// Properties of a Sudoku cell
typedef struct
{
    int cell_num = 0;
    bool filled = false;
    // Sudoku sector
    int sect = 0;
    // Number of numbers that can be in the cell (aka number of trues in valid[n])
    int num_of_valids = n;
    // If index + 1 is a number that can be in the cell
    bool valid[n] = {true, true, true, true, true, true, true, true, true};

}
cell;

//-------------------------------------------------------------------------------------------------------------------->
// FUNCTIONS

bool update_sudoku(cell sudoku[n][n], int i, int j, int val);
void update_only_valid(cell sudoku[n][n], int i, int j);
bool solve_sudoku(cell sudoku[n][n], int i, int j);
void print_sudoku(cell sudoku[n][n]);


//-------------------------------------------------------------------------------------------------------------------->

// Coordinates
typedef struct
{
    int x;
    int y;
}
coord;

// Coordinates of top left cell of each sector
coord sectTop[n] = {{0, 0}, {0, 3}, {0, 6}, {3, 0}, {3, 3}, {3, 6}, {6, 0}, {6, 3}, {6, 6}};


// Assign sect value to all cells
void assign_sect(cell sudoku[n][n])
{
    for (int k = 0; k < n; k++)
    {
        for (int i = sectTop[k].x; i < sectTop[k].x + 3; i++)
        {
            for (int j = sectTop[k].y; j < sectTop[k].y + 3; j++)
            {
                sudoku[i][j].sect = k;
            }
        }
    }
}

string check_inp(string input)
{
    while (input.length() != 81)
    {
        cout << endl << endl << "len of ur input: " << input.length() << endl;
        cout << ":( i need exactly 81 char! Try again:" << endl << "Input here: ";
        cin >> input;
    }

    for (int num = 0; num < 81; num++)
    {
        if (!isdigit(input[num]))
        {
            cout << endl << endl << ":( only digits svp. Try again:" << endl << "Input here: ";
            cin >> input;
            return check_inp(input);
        }
    }
    return input;
}


//-------------------------------------------------------------------------------------------------------------------->

int main(void)
{
    // Make Sudoku template
    cell sudoku[n][n];
    assign_sect(sudoku);

    cout << endl << "Welcome to C-in Sudoku!" << endl <<
         "Provide me an input of 81 numbers, each from 0 to 9 (0 representing empty cell; no separator; from left to right, top to bottom of a sudoku) and I shall return you the solved Sudoku :)"
         << endl << endl << "Input here: ";

    string input;

    cin >> input;

    input = check_inp(input);

    cout << endl;

    int num_hints = 0;

    // Initialize sudoku[n][n]
    int c = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            char inpp = input[c];
            int inp = inpp - '0';

            if (inp != 0)
            {
                num_hints++;
                sudoku[i][j].filled = true;
                sudoku[i][j].cell_num = inp;
                if (!update_sudoku(sudoku, i, j, inp) or !sudoku[i][j].valid[inp - 1])
                {
                    cout << "uh oh ... Sudoku initialization has encountered an error :0 what have you done???" << endl <<
                         "Could it be that you misstyped the Sudoku input? ;-;" << endl;
                    return 1;
                }
            }
            c++;
        }
    }

    if (!solve_sudoku(sudoku, 0, 0))
    {
        cout << "Yikes,,, no solution has been found :')" << endl;
        return 1;
    }

    cout << endl << "Here is the solved Sudoku puzzle" << endl;
    print_sudoku(sudoku);

    cout << "# of Hints Given: " << num_hints << endl;
    if (num_hints < 17)
    {
        cout << "Are you sure this is a proper Sudoku puzzle?? Sudokus with at least 17 hints have a unique solution ..." << endl;
    }

}


//-------------------------------------------->

// Fill cell with the only valid num
void update_only_valid(cell sudoku[n][n], int i, int j)
{
    for (int num = 0; num < n; num++)
    {
        if (sudoku[i][j].valid[num])
        {
            sudoku[i][j].cell_num = num + 1;
            sudoku[i][j].filled = true;
        }
    }
    return;
}

// Eliminate numbers fom valid of cells in the same row, col, sect by Sudoku rules
bool update_sudoku(cell sudoku[n][n], int i, int j, int val)
{
    // 9 num in a row + 9 num in a col + 9 num in a sect - starting cell * 3 = 24 max
    coord update_check[24];
    int count = 0;

    for (int k = 0; k < 9; k++)
    {
        // Update unfilled cells in the row
        if (!sudoku[i][k].filled)
        {
            if (sudoku[i][k].valid[val - 1])
            {
                sudoku[i][k].num_of_valids--;
            }
            sudoku[i][k].valid[val - 1] = false;

            if (sudoku[i][k].num_of_valids == 0)
            {
                return false;
            }
            if (sudoku[i][k].num_of_valids == 1)
            {
                update_check[count].x = i;
                update_check[count].y = k;
                count++;
            }
        }

        // Update unfilled cells in the col
        if (!sudoku[k][j].filled)
        {
            if (sudoku[k][j].valid[val - 1])
            {
                sudoku[k][j].num_of_valids--;
            }
            sudoku[k][j].valid[val - 1] = false;
            if (sudoku[k][j].num_of_valids == 0)
            {
                return false;
            }
            if (sudoku[k][j].num_of_valids == 1)
            {
                update_check[count].x = k;
                update_check[count].y = j;
                count++;
            }

        }

    }

    // Update unfilled cells in the sect
    for (int row = sectTop[sudoku[i][j].sect].x; row < sectTop[sudoku[i][j].sect].x + 3; row++)
    {
        for (int col = sectTop[sudoku[i][j].sect].y; col < sectTop[sudoku[i][j].sect].y + 3; col++)
        {
            if (!sudoku[row][col].filled)
            {
                if (sudoku[row][col].valid[val - 1])
                {
                    sudoku[row][col].num_of_valids--;
                }
                sudoku[row][col].valid[val - 1] = false;
                if (sudoku[row][col].num_of_valids == 0)
                {
                    return false;
                }
                if (sudoku[row][col].num_of_valids == 1)
                {
                    update_check[count].x = row;
                    update_check[count].y = col;
                    count++;
                }

            }

        }
    }

    // Loop through update_check[]
    for (int c = 0; c < count; c++)
    {
        update_only_valid(sudoku, update_check[c].x, update_check[c].y);
        if (!update_sudoku(sudoku, update_check[c].x, update_check[c].y, sudoku[update_check[c].x][update_check[c].y].cell_num))
        {
            return false;
        }
    }
    return true;
}


//-------------------------------------------->

// Copy sudoku
void cpy_sudoku(cell sudokucpy[n][n], cell sudoku[n][n])
{
    for (int p = 0; p < n; p++)
    {
        for (int q = 0; q < n; q++)
        {
            sudokucpy[p][q].sect = sudoku[p][q].sect;
            sudokucpy[p][q].filled = sudoku[p][q].filled;
            sudokucpy[p][q].cell_num = sudoku[p][q].cell_num;
            sudokucpy[p][q].num_of_valids = sudoku[p][q].num_of_valids;
            for (int r = 0; r < n; r++)
            {
                sudokucpy[p][q].valid[r] = sudoku[p][q].valid[r];
            }
        }
    }
    return;
}


// Trial + error + backtracking
bool solve_sudoku(cell sudoku[n][n], int i, int j)
{
    // Find next empty cell
    while (sudoku[i][j].filled and i < n)
    {
        j++;
        // Check next row
        if (j == n)
        {
            i++;
            j = 0;
        }
    }
    // No more empty cells! uwu
    if (i == n)
    {
        return true;
    }

    // Trial + error begins ----------------------->

    cell sudokucpy[n][n];

    for (int v = 0; v < n; v++)
    {
        int val = v + 1;

        // Loop only through valid nums
        if (sudoku[i][j].valid[v])
        {
            // Makes a copy of sudoku
            cpy_sudoku(sudokucpy, sudoku);

            // Try inputting a valid num in an empty cell
            sudokucpy[i][j].cell_num = val;
            sudokucpy[i][j].filled = true;

            // If solves sudoku, sudoku = copy, else copy = previous sudoku (before trial + error) for next trial
            if (update_sudoku(sudokucpy, i, j, val) and solve_sudoku(sudokucpy, i, j))
            {
                cpy_sudoku(sudoku, sudokucpy);
                return true;
            }

        }
    }
    // ======================> backtracking ...
    return false;
}


//-------------------------------------------->

// Output solved sudoku with nice frames in terminal :D
void print_sudoku(cell sudoku[n][n])
{
    cout << "  ——— ——— ———  ——— ——— ———  ——— ——— ——— " << endl;

    cout << " | " << sudoku[0][0].cell_num << " | " << sudoku[0][1].cell_num << " | " << sudoku[0][2].cell_num << " || " <<  sudoku[0][3].cell_num << " | " << sudoku[0][4].cell_num << " | " << sudoku[0][5].cell_num << " || " << sudoku[0][6].cell_num << " | " << sudoku[0][7].cell_num << " | " << sudoku[0][8].cell_num << " | " << endl;
    cout << " |———|———|———||———|———|———||———|———|———| " << endl;
    cout << " | " << sudoku[1][0].cell_num << " | " << sudoku[1][1].cell_num << " | " << sudoku[1][2].cell_num << " || " <<  sudoku[1][3].cell_num << " | " << sudoku[1][4].cell_num << " | " << sudoku[1][5].cell_num << " || " << sudoku[1][6].cell_num << " | " << sudoku[1][7].cell_num << " | " << sudoku[1][8].cell_num << " | " << endl;
    cout << " |———|———|———||———|———|———||———|———|———| " << endl;
    cout << " | " << sudoku[2][0].cell_num << " | " << sudoku[2][1].cell_num << " | " << sudoku[2][2].cell_num << " || " <<  sudoku[2][3].cell_num << " | " << sudoku[2][4].cell_num << " | " << sudoku[2][5].cell_num << " || " << sudoku[2][6].cell_num << " | " << sudoku[2][7].cell_num << " | " << sudoku[2][8].cell_num << " | " << endl;

    cout << " |——— ——— ———  ——— ——— ———  ——— ——— ———| " << endl;

    cout << " | " << sudoku[3][0].cell_num << " | " << sudoku[3][1].cell_num << " | " << sudoku[3][2].cell_num << " || " <<  sudoku[3][3].cell_num << " | " << sudoku[3][4].cell_num << " | " << sudoku[3][5].cell_num << " || " << sudoku[3][6].cell_num << " | " << sudoku[3][7].cell_num << " | " << sudoku[3][8].cell_num << " | " << endl;
    cout << " |———|———|———||———|———|———||———|———|———| " << endl;
    cout << " | " << sudoku[4][0].cell_num << " | " << sudoku[4][1].cell_num << " | " << sudoku[4][2].cell_num << " || " <<  sudoku[4][3].cell_num << " | " << sudoku[4][4].cell_num << " | " << sudoku[4][5].cell_num << " || " << sudoku[4][6].cell_num << " | " << sudoku[4][7].cell_num << " | " << sudoku[4][8].cell_num << " | " << endl;
    cout << " |———|———|———||———|———|———||———|———|———| " << endl;
    cout << " | " << sudoku[5][0].cell_num << " | " << sudoku[5][1].cell_num << " | " << sudoku[5][2].cell_num << " || " <<  sudoku[5][3].cell_num << " | " << sudoku[5][4].cell_num << " | " << sudoku[5][5].cell_num << " || " << sudoku[5][6].cell_num << " | " << sudoku[5][7].cell_num << " | " << sudoku[5][8].cell_num << " | " << endl;

    cout << " |——— ——— ———  ——— ——— ———  ——— ——— ———| " << endl;

    cout << " | " << sudoku[6][0].cell_num << " | " << sudoku[6][1].cell_num << " | " << sudoku[6][2].cell_num << " || " <<  sudoku[6][3].cell_num << " | " << sudoku[6][4].cell_num << " | " << sudoku[6][5].cell_num << " || " << sudoku[6][6].cell_num << " | " << sudoku[6][7].cell_num << " | " << sudoku[6][8].cell_num << " | " << endl;
    cout << " |———|———|———||———|———|———||———|———|———| " << endl;
    cout << " | " << sudoku[7][0].cell_num << " | " << sudoku[7][1].cell_num << " | " << sudoku[7][2].cell_num << " || " <<  sudoku[7][3].cell_num << " | " << sudoku[7][4].cell_num << " | " << sudoku[7][5].cell_num << " || " << sudoku[7][6].cell_num << " | " << sudoku[7][7].cell_num << " | " << sudoku[7][8].cell_num << " | " << endl;
    cout << " |———|———|———||———|———|———||———|———|———| " << endl;
    cout << " | " << sudoku[8][0].cell_num << " | " << sudoku[8][1].cell_num << " | " << sudoku[8][2].cell_num << " || " <<  sudoku[8][3].cell_num << " | " << sudoku[8][4].cell_num << " | " << sudoku[8][5].cell_num << " || " << sudoku[8][6].cell_num << " | " << sudoku[8][7].cell_num << " | " << sudoku[8][8].cell_num << " | " << endl;

    cout << "  ——— ——— ———  ——— ——— ———  ——— ——— ——— " << endl;
    return;
}
